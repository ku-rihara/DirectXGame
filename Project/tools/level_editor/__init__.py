import bpy

#ブレンダーに登録するアドオン情報
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

#モジュールのインポート
from .my_menu import TOPBAR_MT_my_menu
from .stretch_vertex import MYADDON_OT_stretch_vertex
from .collider import OBJECT_PT_collider
from .draw_collider import DrawCollider
from .add_collider import MYADDON_OT_add_collider
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene
from .add_filename import MYADDON_OT_add_filename
from .file_name import OBJECT_PT_file_name
from .ot_disable import MYADDON_OT_Disable
from .pt_disable import OBJECT_PT_Disable
from .pt_emitter import OBJECT_PT_emitter_props
from .ot_emitter import OBJECT_OT_add_emitter, OBJECT_OT_remove_emitter, EmitterItem
from .easing_file import OBJECT_PT_easing_filename
from .easing_step import MYADDON_OT_add_easing_step, MYADDON_OT_remove_easing_step
from .easing_group import MYADDON_OT_add_easing_group, MYADDON_OT_remove_easing_group
from .enemy_spawner import MYADDON_OT_create_spawn_strongEnemy,MYADDON_OT_create_spawn_normalenemy
from .enemy_spawn_symbol import MYADDON_OT_create_spawn_symbol,MYADDON_OT_spawn_import_symbol
from .enemy_spawn_manager import SpawnGroupManager,MYADDON_OT_set_spawn_group, MYADDON_OT_toggle_group_visibility, MYADDON_OT_show_group_info,MYADDON_OT_adjust_spawn_time
from .pt_enemy_spawner import MYADDON_PT_spawn_manager_panel,MYADDON_OT_hide_all_groups, MYADDON_OT_show_all_groups
from .export_spawn_data import MYADDON_OT_export_spawn_data

#Blenderに登録するクラスリスト
classes = (
    TOPBAR_MT_my_menu,
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    MYADDON_OT_add_filename,
    MYADDON_OT_add_collider,
    OBJECT_PT_file_name,
    OBJECT_PT_collider,
    MYADDON_OT_Disable,
    OBJECT_PT_Disable,
    EmitterItem, 
    OBJECT_OT_add_emitter,
    OBJECT_OT_remove_emitter,
    OBJECT_PT_emitter_props,
    OBJECT_PT_easing_filename,#easing
    MYADDON_OT_add_easing_step,
    MYADDON_OT_remove_easing_step,
    MYADDON_OT_add_easing_group,
    MYADDON_OT_remove_easing_group,
    SpawnGroupManager,#Spawner
    MYADDON_OT_set_spawn_group,
    MYADDON_OT_toggle_group_visibility,
    MYADDON_OT_show_group_info,
    MYADDON_OT_spawn_import_symbol,
    MYADDON_OT_create_spawn_symbol,
    MYADDON_OT_create_spawn_normalenemy,
    MYADDON_OT_create_spawn_strongEnemy,
    MYADDON_OT_adjust_spawn_time,
    MYADDON_OT_hide_all_groups,
    MYADDON_OT_show_all_groups,
    MYADDON_PT_spawn_manager_panel,
    MYADDON_OT_export_spawn_data,
)

#アドオン有効化時コールバック
def register():
    print("=== レベルエディタ登録開始 ===")
    
    #Blenderにクラスを登録
    for cls in classes:
        try:
            bpy.utils.register_class(cls)
            print(f"登録成功: {cls.__name__}")
        except Exception as e:
            print(f"登録失敗: {cls.__name__} - {e}")
    
    # EmitterItemが登録された後にプロパティを定義
    try:
        # 既存のプロパティがあれば削除
        if hasattr(bpy.types.Object, 'emitter_list'):
            del bpy.types.Object.emitter_list
        
        bpy.types.Object.emitter_list = bpy.props.CollectionProperty(
            type=EmitterItem,
            name="Emitter List",
            description="オブジェクトのEmitterリスト"
        )
        print("emitter_listプロパティを登録しました")
    except Exception as e:
        print(f"プロパティ登録失敗: {e}")
    
    #メニュー項目追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider, (), "WINDOW", "POST_VIEW")
    
    print("レベルエディタが有効化されました")

#アドオン無効化時コールバック
def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    
    #3Dビューから描画関数を削除
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle, "WINDOW")
    
    # プロパティを削除
    if hasattr(bpy.types.Object, 'emitter_list'):
        del bpy.types.Object.emitter_list
    
    #Blenderからクラスを登録解除
    for cls in reversed(classes):  # 登録とは逆順で解除
        bpy.utils.unregister_class(cls)

    print("レベルエディタが無効化されました")