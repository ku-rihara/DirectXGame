import bpy 
import os 
import bpy.ops 

from .enemy_spawn_manager import SpawnGroupManager

class SpawnNames(): 
    #インデックス 
    PROTOTYPE = 0  
    INSTANCE = 1 
    FILENAME = 2 
 
    names = {} 
    names["NormalEnemy"] = ("PrototypeNormalEnemySpawn", "NormalEnemySpawn", "Models/NormalEnemy/NormalEnemy.obj") 
    names["StrongEnemy"] = ("PrototypeStrongEnemySpawn", "StrongEnemySpawn", "Models/StrongEnemy/StrongEnemy.obj") 


class MYADDON_OT_spawn_import_symbol(bpy.types.Operator): 
    bl_idname = "myaddon.spawn_import_symbol" 
    bl_label = "出現ポイントシンボルImport" 
    bl_description = "出現ポイントのシンボルをImportします" 
    prototype_object_name = "PrototypeNormalEnemySpawn" 
    object_type = "EnemySpawn" 
 
    type: bpy.props.StringProperty(name="Type", default="")
 
    def load_obj(self, type): 
        # 重複ロード防止 
        spawn_object = bpy.data.objects.get(SpawnNames.names[type][SpawnNames.PROTOTYPE]) 
        if spawn_object is not None: 
            return {'CANCELLED'} 
 
        # アドオンディレクトリ 
        addon_directory = os.path.dirname(os.path.abspath(__file__)) 
        relative_path = SpawnNames.names[type][SpawnNames.FILENAME] 
        full_path = os.path.join(addon_directory, relative_path) 
 
        if not os.path.exists(full_path): 
            self.report({'ERROR'}, f"OBJファイルが見つかりません: {full_path}") 
            return {'CANCELLED'} 
 
        # OBJインポート 
        bpy.ops.wm.obj_import('EXEC_DEFAULT', filepath=full_path, display_type='THUMBNAIL', forward_axis='Z', up_axis='Y') 
        bpy.ops.object.transform_apply(location=False, rotation=True, scale=False) 
 
        obj = bpy.context.active_object 
        if obj is None: 
            self.report({'ERROR'}, "アクティブオブジェクトが取得できません") 
            return {'CANCELLED'} 
 
        # オブジェクトの名前とタイプを設定 
        obj.name = SpawnNames.names[type][SpawnNames.PROTOTYPE] 
        obj[type] = MYADDON_OT_spawn_import_symbol.object_type 
 
        return {'FINISHED'} 
 
    def execute(self, context): 
        # 特定のタイプが指定されている場合は、そのタイプのみをロード
        if self.type:
            self.load_obj(self.type)
        else:
            # タイプが指定されていない場合は全てをロード
            self.load_obj("NormalEnemy") 
            self.load_obj("StrongEnemy") 
 
        return {'FINISHED'} 

class MYADDON_OT_create_spawn_symbol(bpy.types.Operator): 
    bl_idname = "myaddon.create_spawn_symbol" 
    bl_label = "出現ポイントシンボル作成" 
    bl_description = "出現ポイントのシンボルを作成します" 
    bl_options = {'REGISTER', 'UNDO'} 
 
    type: bpy.props.StringProperty(name="Type", default="NormalEnemy") 
 
    def execute(self, context): 
        print("出現ポイントのシンボルを作成します") 
 
        bpy.ops.object.select_all(action='DESELECT') 
 
        # プロトタイプオブジェクトを取得
        prototype_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE]) 
        if prototype_object is None: 
            # 必要なタイプのみをロード
            bpy.ops.myaddon.spawn_import_symbol('EXEC_DEFAULT', type=self.type)  
            prototype_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE]) 
 
        if prototype_object is None:
            self.report({'ERROR'}, f"プロトタイプオブジェクトが見つかりません: {SpawnNames.names[self.type][SpawnNames.PROTOTYPE]}")
            return {'CANCELLED'}

        # プロトタイプオブジェクトを複製してインスタンスを作成
        prototype_object.select_set(True)
        bpy.context.view_layer.objects.active = prototype_object
        bpy.ops.object.duplicate()
        
        # 複製されたオブジェクトの名前を変更
        new_instance = bpy.context.active_object
        new_instance.name = SpawnNames.names[self.type][SpawnNames.INSTANCE]
        
        # 敵の種類とスポーングループのプロパティを設定
        new_instance["enemy_type"] = self.type
        new_instance["spawn_group"] = 0  # デフォルト値
        new_instance["spawn_offset"] = 0.0
 
        return {'FINISHED'} 

