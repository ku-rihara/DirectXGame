import bpy

class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname="myaddon.myaddon_ot_create_object"
    bl_label="ICO球生成"
    bl_description="ICO球を生成します"
    #リドゥ、アンドゥ可能オプション
    bl_options = {'REGISTER', 'UNDO'}

    #メニューを実行した時に呼ばれるコールバック関数
    def execute(self,context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました")
        #オペレータの命令終了の通知
        return {'FINISHED'}
