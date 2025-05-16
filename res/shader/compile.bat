"../../3rd/dxc/bin/x64/dxc.exe" -E vs -T vs_6_0 -I . -Fo box/vert.dxil box/box.hlsl
"../../3rd/dxc/bin/x64/dxc.exe" -E ps -T ps_6_0 -I . -Fo box/frag.dxil box/box.hlsl
"../../3rd/dxc/bin/x64/dxc.exe" -E vs -T vs_6_0 -I . -spirv -Fo box/vert.spv box/box.hlsl
"../../3rd/dxc/bin/x64/dxc.exe" -E ps -T ps_6_0 -I . -spirv -Fo box/frag.spv box/box.hlsl