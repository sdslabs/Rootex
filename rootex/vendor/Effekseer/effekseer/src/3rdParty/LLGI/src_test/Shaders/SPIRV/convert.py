import glob
import subprocess
import os

verts = glob.glob('../HLSL_DX12/*.vert', recursive=True)
frags = glob.glob('../HLSL_DX12/*.frag', recursive=True)

for f in (verts + frags):
    subprocess.call(['glslangValidator', '-D', f, '-e', 'main', '-V', '-o', os.path.basename(
        f) + '.spv', '--shift-sampler-binding', '1', '--shift-texture-binding', '1', '--invert-y'])
