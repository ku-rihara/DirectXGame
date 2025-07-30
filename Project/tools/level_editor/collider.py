import bpy

from .add_collider import MYADDON_OT_add_collider

class OBJECT_PT_collider(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname="OBJECT_PT_collider"
    bl_label="Collider"
    bl_space_type="PROPERTIES"
    bl_region_type="WINDOW"
    bl_context="object"

    #サブメニューの描画
    def draw(self,context):

        #パネルに項目を追加
       if "collider" in context.object:
           #既にプロパティが存在してれば、プロパティを追加
           self.layout.prop(context.object,'["collider"]',text="Type")
           self.layout.prop(context.object,'["collider_center"]',text="Center")
           self.layout.prop(context.object,'["collider_size"]',text="Size")
       else:
           #プロパティが無ければ、プロパティを追加棒んを追加
           self.layout.operator(MYADDON_OT_add_collider.bl_idname)
           

