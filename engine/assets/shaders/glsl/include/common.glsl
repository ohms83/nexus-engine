/**
* @brief Calculates the normal matrix from the model matrix.
* @param modelMtx The model matrix.
* @return The normal matrix.
*/
mat3 GetNormalMatrix(mat4 modelMtx)
{
    // Use mat3 to ignore translation and handle non-uniform scaling correctly
    return mat3(transpose(inverse(modelMtx)));
}

/**
 * @brief Calculates the TBN matrix for transforming normals from tangent space to world space.
 * @param tangent The tangent vector in object space.
 * @param normal The normal vector in object space.
 * @param modelMtx The model matrix.
 * @return The TBN matrix.
 */
mat3 GetTBNMatrix(vec3 tangent, vec3 normal, mat3 modelMtx)
{
    // Transform TBN vectors to the same space as our lighting vectors (world space)
    tangent = normalize(modelMtx * tangent);
    normal = normalize(modelMtx * normal);

    // Re-orthogonalize T with respect to N (Gram-Schmidt process)
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    // Recalculate Bitangent to ensure it's orthonormal
    vec3 bitangent = cross(normal, tangent);
    
    return mat3(tangent, bitangent, normal);
}

/**
 * @brief Transforms a normal vector from tangent space to world space.
 * @param normal The normal vector in tangent space.
 * @param modelMtx The model matrix.
 * @return The normal vector in world space.
 */
vec3 TransformNormalToWorldSpace(vec3 normal, mat4 modelMtx)
{
    // Use mat3 to ignore translation and handle non-uniform scaling correctly
    mat3 normalMatrix = GetNormalMatrix(modelMtx);
    return normalize(normalMatrix * normal);
}

/**
 * @brief Transforms a normal vector sampled from a normal map to world space.
 * @param normalmap The normal map texture.
 * @param uv The texture coordinates.
 * @param TBN The TBN matrix.
 * @return The normal vector in world space.
 */
vec3 SampleNormalmapToWorldSpace(sampler2D normalmap, vec2 uv, mat3 TBN)
{
    vec3 normal = texture2D(normalmap, uv).xyz;
    // The texture stores normal vectors as colors (0 to 1). We convert them
    // back to the standard vector range (-1 to 1).
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    return normal;
}