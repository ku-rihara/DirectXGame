import bpy

class MYADDON_OT_Disable(bpy.types.Operator):
    bl_idname="myaddon.myaddon_ot_disable"
    bl_label="isDisable"
    bl_description="['isDisable']カスタムプロパティを追加します"
    bl_options={"REGISTER","UNDO"}


    #サブメニューの描画
    def execute(self,context):
        #['isDisable']カスタムプロパティを追加
        context.object["isDisable"]=False
        return {"FINISHED"}


