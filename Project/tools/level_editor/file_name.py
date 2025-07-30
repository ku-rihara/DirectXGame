import bpy

from .add_filename import MYADDON_OT_add_filename

class OBJECT_PT_file_name(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname="object_file_name"
    bl_label="FileName"
    bl_space_type="PROPERTIES"
    bl_region_type="WINDOW"
    bl_context="object"

    #サブメニューの描画
    def draw(self,context):

        #パネルに項目を追加
       if "file_name" in context.object:
           #既にプロパティが存在してれば、プロパティを追加
           self.layout.prop(context.object,'["file_name"]', text = self.bl_label)
       else:
           #プロパティが無ければ、プロパティを追加棒んを追加
           self.layout.operator(MYADDON_OT_add_filename.bl_idname)
           

