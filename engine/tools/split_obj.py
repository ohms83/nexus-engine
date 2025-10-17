import argparse
import os
from pathlib import Path
import math

positions = []
texcoords = []
normals = []
meshes = []
material = ""

T_COMMENT = "#"
T_VERTEX = "v"
T_TEXCOORD = "vt"
T_NORMAL = "vn"
T_FACE = "f"
T_MESH = "g"
T_SMOOTH = "s"
T_MTL = "usemtl"
T_MTL_LIB = "mtllib"

parser = argparse.ArgumentParser(description="Spliting an OBJ model into multiple files.")
parser.add_argument("filename", help="Path to the target OBJ file.")
# parser.add_argument("objects", nargs="+", help="A list of the group of object that will be extracted to the output file.")
# parser.add_argument("--output", "-o", help="Output filename.")

class Vector:
    def __init__(self, str_tokens):
        if len(str_tokens) != 3:
            raise RuntimeError(f"Invalid tokens={str_tokens}")
        self.x = float(str_tokens[0])
        self.y = float(str_tokens[1])
        self.z = float(str_tokens[2])
    def __str__(self):
        return f"{self.x:.4f} {self.y:.4f} {self.z:.4f}"
    def __repr__(self):
        return f"<{self.x:.4f} {self.y:.4f} {self.z:.4f}>"
    def __eq__(self, value):
        return math.isclose(self.x, value.x) and math.isclose(self.y, value.y) and math.isclose(self.z, value.z)

class Vertex:
    def __init__(self, vertex_data: str):
        tokens = vertex_data.strip().split("/")
        if len(tokens) != 3:
            raise RuntimeError("Invalid vertex data: {vertex_data}")
        self.position = int(tokens[0])
        self.texcoord = int(tokens[1])
        self.normal = int(tokens[2])
    def __str__(self):
        return f"{self.position}/{self.texcoord}/{self.normal}"

class PolyFace:
    def __init__(self, string_tokens):
        self.num_vertex = len(string_tokens)
        if self.num_vertex < 3:
            raise RuntimeError(f"Invalid string tokens: {string_tokens}")
        self.v1 = Vertex(string_tokens[0])
        self.v2 = Vertex(string_tokens[2])
        self.v3 = Vertex(string_tokens[2])
        if self.num_vertex == 4:
            self.v4 = Vertex(string_tokens[3])
    def __str__(self):
        if self.num_vertex == 4:
            return f"f {self.v1} {self.v2} {self.v3} {self.v4}"
        else:
            return f"f {self.v1} {self.v2} {self.v3}"

class SurfaceGroup:
    def __init__(self, id: int):
        self.id = id
        self.polygons = []
    def __str__(self):
        output = f"s {self.id}\n"
        for polygon in self.polygons:
            if type(polygon) != PolyFace:
                raise TypeError(f"Invalid polygon type {type(polygon)}")
            output += f"{polygon}\n"
        return output

class Mesh():
    def __init__(self, name: str, in_file):
        self.vertex_positions = []
        self.vertex_texcoords = []
        self.vertex_normals = []
        self.surface_groups = []
        self.usemat = ""
        self.name = name

        position_dict = {}
        texcoord_dict = {}
        normal_dict = {}
        # Current surface group
        surface_group = None

        for line in in_file:
            line = line.strip()
            print(f"{line}")

            tokens = line.split()
            if len(tokens) == 0 or tokens[0] == T_COMMENT:
                continue

            tag = tokens[0]
            if tag == T_FACE:
                polygon = PolyFace(tokens[1:])
                print(f"xx {polygon}")
                polygon.v1 = self.localize_vertex(polygon.v1, position_dict, texcoord_dict, normal_dict)
                polygon.v2 = self.localize_vertex(polygon.v2, position_dict, texcoord_dict, normal_dict)
                polygon.v3 = self.localize_vertex(polygon.v3, position_dict, texcoord_dict, normal_dict)
                print(f"{polygon}\n")

                if not surface_group:
                    surface_group = SurfaceGroup(0)
                surface_group.polygons.append(polygon)
            elif tag == T_SMOOTH:
                surface_group = SurfaceGroup(int(tokens[1]))
                self.surface_groups.append(surface_group)
            elif tag == T_MTL:
                self.usemat = tokens[1]
            else:
                return

    def localize_vertex(self, vertex: Vertex, position_dict: dict, texcoord_dict: dict, normal_dict: dict):
        vertex.position = Mesh.map_to_local_index(
            vertex.position, positions, self.vertex_positions, position_dict)
        vertex.texcoord = Mesh.map_to_local_index(
            vertex.texcoord, texcoords, self.vertex_texcoords, texcoord_dict)
        vertex.normal = Mesh.map_to_local_index(
            vertex.normal, normals, self.vertex_normals, normal_dict)
        return vertex
        
    def map_to_local_index(index: int, global_list: list, local_list: list, index_map: dict):
        local_index = 0
        if index in index_map:
            local_index = index_map[index]
        else:
            try:
                local_list.append(global_list[index-1])
            except IndexError:
                raise IndexError(f"Index out of bound! index={index} global_list[{len(global_list)}]")

            local_index = len(local_list)
            index_map[index] = local_index
        return local_index

    def write_to_file(self, out_path: str):
        filepath = os.path.join(out_path, f"{self.name}.obj")
        print(f"Writing {filepath}")

        with open(filepath, "w") as out_file:
            out_file.write(f"mtllib {material}\n\n")
            
            for position in self.vertex_positions:
                out_file.write(f"v {position}\n")
            out_file.write(f"# {len(self.vertex_positions)} vertices\n\n")

            for normal in self.vertex_normals:
                out_file.write(f"vn {normal}\n")
            out_file.write(f"# {len(self.vertex_normals)} vertex normals\n\n")

            for texcoord in self.vertex_texcoords:
                out_file.write(f"vt {texcoord}\n")
            out_file.write(f"# {len(self.vertex_texcoords)} vertex texcoords\n\n")

            out_file.write(f"g {self.name}\n")
            out_file.write(f"usemtl {self.usemat}\n")

            for surface_group in self.surface_groups:
                out_file.write(f"{surface_group}\n")
            out_file.write("\n")

args = parser.parse_args()
filepath = Path(args.filename).resolve()
print(f"Parsing {filepath}")

with open(filepath) as in_file:
    for line in in_file:
        tokens = line.strip().split()
        if len(tokens) == 0 or tokens[0] == T_COMMENT:
            continue

        # print(f"{line}")
        tag = tokens[0]
        if tag == T_VERTEX:
            positions.append(Vector(tokens[1:]))
        elif tag == T_TEXCOORD:
            texcoords.append(Vector(tokens[1:]))
        elif tag == T_NORMAL:
            normals.append(Vector(tokens[1:]))
        elif tag == T_MTL_LIB:
            material = tokens[1]
        elif tag == T_MESH:
            meshes.append(Mesh(tokens[1], in_file))
            break

for mesh in meshes:
    outpath, _ = os.path.split(filepath)
    mesh.write_to_file(outpath)