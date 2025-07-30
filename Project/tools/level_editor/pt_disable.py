import bpy

from .ot_disable import MYADDON_OT_Disable

class OBJECT_PT_Disable(bpy.types.Panel):
    bl_idname = "OBJECT_PT_Disable"
    bl_label = "isDisable"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        layout = self.layout
        obj = context.object

        if "isDisable" in obj:
            layout.prop(obj, '["isDisable"]', text=self.bl_label)
        else:
            layout.operator(MYADDON_OT_Disable.bl_idname)
