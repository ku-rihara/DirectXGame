import bpy

bl_info = {
    "name": "レベルエディタ",
    "author": "Keta Kurihara",
    "version": (1, 0),
    "blender": (4, 1),
    "location": "",
    "warning": "",
    "description": "レベルエディタ",
    "wiki_url": "",
    "tracler_url": "",
    "category": "Object"
}

from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene
from .stretch_vertex import MYADDON_OT_stretch_vertex
from .enemy_spawner import MYADDON_OT_create_spawn_normalenemy, MYADDON_OT_create_spawn_strongEnemy


class TOPBAR_MT_my_menu(bpy.types.Menu):
   #Blenderがクラスを識別する為の固有の文字列
    bl_idname = "TOPBAR_MT_my_menu"
    #メニューのラベルとして表示される文字列
    bl_label = "My Menu"
    #著者表示用の文字列
    be_description="拡張メニュー by"+bl_info["author"]

    #サブメニューの描画
    def draw(self, context):
        #トップバーの「エディターメニュー」に項目を追加
        self.layout.operator("wm.url_open_preset", 
            text="Manual",icon='HELP')
        #オペレータを追加
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname, 
             text=MYADDON_OT_stretch_vertex.bl_label,)
        
        #オペレータを追加
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname, 
            text=MYADDON_OT_create_ico_sphere.bl_label,)
        
         #オペレータを追加
        self.layout.operator(MYADDON_OT_export_scene.bl_idname, 
            text=MYADDON_OT_export_scene.bl_label,)
        
         #オペレータを追加
        self.layout.operator(MYADDON_OT_create_spawn_normalenemy.bl_idname, 
            text=MYADDON_OT_create_spawn_normalenemy.bl_label,)
        
         #オペレータを追加
        self.layout.operator(MYADDON_OT_create_spawn_strongEnemy.bl_idname, 
            text=MYADDON_OT_create_spawn_strongEnemy.bl_label,)

    #既存のメニューにサブメニューを追加
    def submenu(self,context):
        #IDを指定してメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)