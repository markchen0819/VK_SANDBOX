setlocal

REM Define the paths
set "VULKAN_SDK_PATH=%~dp0..\..\..\Libraries\VulkanSDK\1.3.243.0\Bin"
set "SHADER_SOURCE_PATH=%~dp0Sources"
set "SPV_OUTPUT_PATH=%~dp0"

REM -----------------------------------------
REM Compile Base Shaders
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\shader.vert" -o "%SPV_OUTPUT_PATH%\vert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\shader.frag" -o "%SPV_OUTPUT_PATH%\frag.spv"
REM -----------------------------------------
REM Compile Animation Shaders
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\skeletalShader.vert" -o "%SPV_OUTPUT_PATH%\skeletalvert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\skeletalShader.frag" -o "%SPV_OUTPUT_PATH%\skeletalfrag.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\debugBoneShader.vert" -o "%SPV_OUTPUT_PATH%\debugbonevert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\debugBoneShader.frag" -o "%SPV_OUTPUT_PATH%\debugbonefrag.spv"
REM -----------------------------------------
REM Compile Line Renderer Shaders
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\lineRendererShader.vert" -o "%SPV_OUTPUT_PATH%\linerenderervert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\lineRendererShader.frag" -o "%SPV_OUTPUT_PATH%\linerendererfrag.spv"
REM -----------------------------------------
REM Compile Particle Shaders
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\renderParticleShader.vert" -o "%SPV_OUTPUT_PATH%\renderparticlevert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\renderParticleShader.frag" -o "%SPV_OUTPUT_PATH%\renderparticlefrag.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\computeParticleShader.comp" -o "%SPV_OUTPUT_PATH%\computeparticlecomp.spv"
REM -----------------------------------------
REM Compile Grass Shaders
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\renderGrassShader.vert" -o "%SPV_OUTPUT_PATH%\rendergrassvert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\renderGrassShader.frag" -o "%SPV_OUTPUT_PATH%\rendergrassfrag.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\computeGrassShader.comp" -o "%SPV_OUTPUT_PATH%\computegrasscomp.spv"
REM -----------------------------------------
REM Compile Fluid Shaders
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\renderFluidParticleShader.vert" -o "%SPV_OUTPUT_PATH%\renderfluidparticlevert.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\renderFluidParticleShader.frag" -o "%SPV_OUTPUT_PATH%\renderfluidparticlefrag.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\computeFluidDensityPressureShader.comp" -o "%SPV_OUTPUT_PATH%\computefluiddensitypressurecomp.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\computeFluidForceShader.comp" -o "%SPV_OUTPUT_PATH%\computefluidforcecomp.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\computeFluidIntegrateShader.comp" -o "%SPV_OUTPUT_PATH%\computefluidintegratecomp.spv"
"%VULKAN_SDK_PATH%\glslc.exe" "%SHADER_SOURCE_PATH%\computeFluidCopyShader.comp" -o "%SPV_OUTPUT_PATH%\computefluidcopycomp.spv"
pause
