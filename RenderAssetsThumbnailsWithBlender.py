import bpy
import os
import shutil

working_dir = "F:/Development/LookTheDungeon"
models_directories = ["/Assets/Models/Characters", "/Assets/Models/LevelParts"]
previews_output_dir = "/Assets/ModelsPreviews"
allowed_types = ["glb"]
objects_collection = bpy.data.collections["ObjectToRender"]

for model_dir in models_directories:
    categoryName = model_dir.split("/")[3]

    for file in os.listdir(working_dir + model_dir):
        for allowed_type in allowed_types:
            if file.endswith(allowed_type):
                # Import the model into ObjectToRender collection
                bpy.ops.import_scene.gltf(filepath=working_dir + model_dir + "/" + file)
                objects_collection.objects.link(bpy.context.selected_objects[0])

                # Center camera on the object
                bpy.ops.view3d.camera_to_view_selected()

                # Save the render in jpg format
                bpy.context.scene.render.filepath = working_dir + previews_output_dir + "/" + categoryName + "/" + file.replace(".gltf.glb", ".png")
                bpy.context.scene.render.filepath = bpy.context.scene.render.filepath.replace(".glb", ".png")
                bpy.ops.render.render(write_still = True)

                # Clear all objects except lights and cameras
                for obj in bpy.data.objects:
                    if obj.type != 'LIGHT' and obj.type != 'CAMERA':
                        bpy.data.objects.remove(obj, do_unlink=True)