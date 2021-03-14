import glob
import subprocess
import os

verts = glob.glob('HLSL_DX12/*.vert', recursive=True)
frags = glob.glob('HLSL_DX12/*.frag', recursive=True)

for f in (verts):
    subprocess.call(['ShaderTranspiler', '--vert', '-M', '--input', f, '--output', 'Metal/' + os.path.basename(f)])

for f in (frags):
    subprocess.call(['ShaderTranspiler', '--frag', '-M', '--input', f, '--output', 'Metal/' + os.path.basename(f)])


for f in (verts):
    subprocess.call(['ShaderTranspiler', '--vert', '-V', '--input', f, '--output', 'GLSL_VULKAN/' + os.path.basename(f)])

for f in (frags):
    subprocess.call(['ShaderTranspiler', '--frag', '-V', '--input', f, '--output', 'GLSL_VULKAN/' + os.path.basename(f)])

for f in (verts):
    subprocess.call(['ShaderTranspiler', '--vert', '-G', '--input', f, '--output', 'GLSL_GL/' + os.path.basename(f)])

for f in (frags):
    subprocess.call(['ShaderTranspiler', '--frag', '-G', '--input', f, '--output', 'GLSL_GL/' + os.path.basename(f)])


verts = glob.glob('GLSL_VULKAN/*.vert', recursive=True)
frags = glob.glob('GLSL_VULKAN/*.frag', recursive=True)

for f in (verts + frags):
    subprocess.call(['glslangValidator', f, '-e', 'main', '-V', '-o', 'SPIRV/' + os.path.basename(f) + '.spv'])
