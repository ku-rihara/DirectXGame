import bpy

class MYADDON_OT_add_filename(bpy.types.Operator):
    bl_idname="myaddon.myaddon_ot_add_filename"
    bl_label="FileName 追加"
    bl_description="['file_name']カスタムプロパティを追加します"
    bl_options={"REGISTER","UNDO"}


    #サブメニューの描画
    def execute(self,context):
        #['file_name']カスタムプロパティを追加
        context.object["file_name"]=""
        return {"FINISHED"}

