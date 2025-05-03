"../../3rd/dxc/bin/x64/dxc.exe" -E main -T vs_6_0 -I . -Fo box/vert.dxil box/vert.hlsl
"../../3rd/dxc/bin/x64/dxc.exe" -E main -T ps_6_0 -I . -Fo box/frag.dxil box/frag.hlsl