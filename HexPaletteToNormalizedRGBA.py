import pyperclip

colors = pyperclip.paste()
colors = colors.replace("\n", " ").strip()

def color_hex_to_rgba(hex):
    r = int(hex[1:3], 16)
    g = int(hex[3:5], 16)
    b = int(hex[5:7], 16)
    a = 255
    return (r / 255, g / 255, b / 255, a / 255)

# Convert colors to RGBA normalized floats
colors = [color_hex_to_rgba(color) for color in colors.split(" ")]

# Copy glsl code to clipboard
glslCode = "const int paletteSize = {};\n" . format(len(colors))
glslCode += "vec4 palette[paletteSize] = vec4[] (\n"

for color in colors:
    glslCode += "    vec4({}, {}, {}, {})" . format(color[0], color[1], color[2], color[3])

    if colors.index(color) != len(colors) - 1:
        glslCode += ",\n"
    else:
        glslCode += "\n"

glslCode += ");\n\n"

pyperclip.copy(glslCode)
print("GLSL code copied to clipboard")

