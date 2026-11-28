RenderDoc Capture Instructions

This document explains how to capture a GPU frame using RenderDoc on Windows and correlate it with Tracy + the in-engine GraphicsProfiler.

Prerequisites
- Install RenderDoc (https://renderdoc.org/) and launch the RenderDoc UI.
- Build the engine in Debug (or any build with symbols).
- Build in Debug or RelWithDebInfo to enable Tracy CPU instrumentation.
- Optionally configure `NXS_ENABLE_TRACY_GPU_PROFILING=ON` to enable Tracy OpenGL GPU timer queries.

Capturing a frame
1. Start RenderDoc.
2. Click "Launch Application" (or "Attach to running process" if already running).
   - Executable: point to your built Nexus editor/game EXE in the `build/` or `cmake-build-debug/bin/` folder.
   - Working directory: the folder where the exe expects to find assets (project root or `build/bin`).
   - Optional: set program arguments if needed.
3. Click "Launch". The application will start under RenderDoc.
4. Reproduce the frame/scene you want to analyze in the running application.
5. In RenderDoc, press the "Capture Frame(s)" button (F12 by default) or use the UI "Capture Frame" button while the app is active.
6. After capture completes, open the captured frame in RenderDoc for analysis.

What to look for in RenderDoc
- Per-draw GPU time and shader durations (in the Event Browser / API Inspector).
- Overdraw and pixel shader invocation counts in the Pipeline State view.
- Resource bindings per draw (bound textures, samplers, VAO/VBO, shader program). This helps find excessive state changes.
- Draw call history: sort by GPU time or by shader to find heavy draws.

Correlating with Tracy and GraphicsProfiler
- Tracy: start the Tracy Profiler and connect to the running application to view CPU zones and frame boundaries.
  - Render passes are represented by named Tracy sections in `SceneRenderer::Render`.
  - Enable `NXS_ENABLE_TRACY_GPU_PROFILING` to collect Tracy OpenGL GPU zones.
- GraphicsProfiler: the engine now logs a compact per-frame summary via the `GraphicsProfiler` (LOG_INFO). The log line looks like:
  "Frame Composition: drawCalls=..., uniquePrograms=..., uniqueTextures=..., indices=..., programBindEvents=..., textureBindEvents=..."
  - This helps identify whether the bottleneck is many program/textures binds or many small draw calls.

Recommended workflow
1. Start Tracy Profiler and begin a capture session if CPU or GPU zone correlation is needed.
2. Run RenderDoc capture for a troublesome frame.
3. Open RenderDoc and inspect the heavy draws and resource bindings.
4. Cross-reference RenderDoc's heavy draw indices/shader names with Tracy's per-pass timings to identify the culprit.
5. Use the `GraphicsProfiler` log to get a quick per-frame overview of draw-call composition and frequent program/texture binds.

Optional: programmatic capture
- RenderDoc's `renderdoc_app.h` provides an API to start captures programmatically. If you want an in-engine "capture next frame" button, add RenderDoc's loader and call `RENDERDOC_GetAPI()` and `StartFrameCapture()/EndFrameCapture()` when requested.

Notes
- For NVIDIA GPUs, you may also use Nsight or NVIDIA Aftermath for additional metrics.
- Collect multiple frames to ensure you sample representative workload.

*** End of file
