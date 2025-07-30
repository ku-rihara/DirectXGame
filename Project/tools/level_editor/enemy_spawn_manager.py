import bpy 
import json

class SpawnGroupManager:
    """スポーングループの管理クラス"""
    
    # グループ情報をテキストブロックに保存するためのキー
    STORAGE_KEY = "spawn_groups_data"
    
    @staticmethod
    def _load_group_data():
        """テキストブロックからグループデータを読み込み"""
        if SpawnGroupManager.STORAGE_KEY in bpy.data.texts:
            try:
                text_data = bpy.data.texts[SpawnGroupManager.STORAGE_KEY].as_string()
                return json.loads(text_data) if text_data else {}
            except (json.JSONDecodeError, AttributeError):
                return {}
        return {}
    
    @staticmethod
    def _save_group_data(data):
        """グループデータをテキストブロックに保存"""
        if SpawnGroupManager.STORAGE_KEY not in bpy.data.texts:
            bpy.data.texts.new(SpawnGroupManager.STORAGE_KEY)
        
        text_block = bpy.data.texts[SpawnGroupManager.STORAGE_KEY]
        text_block.clear()
        text_block.write(json.dumps(data, indent=2))
    
    @staticmethod
    def get_group_spawn_time(group_id):
        """グループの生成タイミング時間を取得"""
        data = SpawnGroupManager._load_group_data()
        return data.get(str(group_id), {}).get("spawn_time", 0.0)
    
    @staticmethod
    def set_group_spawn_time(group_id, spawn_time):
        """グループの生成タイミング時間を設定"""
        data = SpawnGroupManager._load_group_data()
        
        # グループデータを更新
        if str(group_id) not in data:
            data[str(group_id)] = {}
        
        data[str(group_id)]["spawn_time"] = spawn_time
        
        # テキストブロックに保存
        SpawnGroupManager._save_group_data(data)
        
        # 動的プロパティも更新（UI表示用）
        scene = bpy.context.scene
        prop_name = f"spawn_group_{group_id}_time"
        
        # プロパティが定義されていない場合は定義
        if not hasattr(bpy.types.Scene, prop_name):
            setattr(bpy.types.Scene, prop_name, 
                bpy.props.FloatProperty(
                    name=f"Group {group_id} Spawn Time",
                    default=0.0,
                    min=0.0,
                    unit='TIME',
                    update=lambda self, context: SpawnGroupManager._on_property_update(group_id, getattr(self, prop_name))
                ))
        
        # プロパティの値をシーンに設定
        setattr(scene, prop_name, spawn_time)
    
    @staticmethod
    def _on_property_update(group_id, new_value):
        """UIプロパティが更新された時の処理"""
        # テキストブロックの値も更新
        data = SpawnGroupManager._load_group_data()
        if str(group_id) not in data:
            data[str(group_id)] = {}
        data[str(group_id)]["spawn_time"] = new_value
        SpawnGroupManager._save_group_data(data)

    @staticmethod
    def get_object_spawn_time(obj):
        group_id = obj.get("spawn_group", 0)
        offset = obj.get("spawn_offset", 0.0)
        return SpawnGroupManager.get_group_spawn_time(group_id) + offset

    @staticmethod
    def get_existing_groups():
        """既存のスポーングループを取得"""
        groups = set()
        for obj in bpy.data.objects:
            if "spawn_group" in obj:
                groups.add(obj["spawn_group"])
        return sorted(list(groups))
    
    @staticmethod
    def get_objects_in_group(group_id):
        """指定グループのオブジェクトを取得"""
        objects = []
        for obj in bpy.data.objects:
            if "spawn_group" in obj and obj["spawn_group"] == group_id:
                objects.append(obj)
        return objects
    
    @staticmethod
    def set_group_visibility(group_id, visible):
        """グループの表示/非表示を設定"""
        objects = SpawnGroupManager.get_objects_in_group(group_id)
        for obj in objects:
            obj.hide_viewport = not visible
            obj.hide_render = not visible
    
    @staticmethod
    def restore_properties():
        """保存されたデータからプロパティを復元"""
        data = SpawnGroupManager._load_group_data()
        for group_id_str, group_data in data.items():
            group_id = int(group_id_str)
            spawn_time = group_data.get("spawn_time", 0.0)
            
            # 動的プロパティを再作成
            scene = bpy.context.scene
            prop_name = f"spawn_group_{group_id}_time"
            
            # プロパティを定義
            if not hasattr(bpy.types.Scene, prop_name):
                setattr(bpy.types.Scene, prop_name, 
                    bpy.props.FloatProperty(
                        name=f"Group {group_id} Spawn Time",
                        default=0.0,
                        min=0.0,
                        unit='TIME',
                        update=lambda self, context, gid=group_id: SpawnGroupManager._on_property_update(gid, getattr(self, f"spawn_group_{gid}_time"))
                    ))
            
            # プロパティの値を設定
            setattr(scene, prop_name, spawn_time)


# グループ管理用の新しいオペレーター
class MYADDON_OT_toggle_group_visibility(bpy.types.Operator):
    bl_idname = "myaddon.toggle_group_visibility"
    bl_label = "グループ表示切り替え"
    bl_description = "指定したグループの表示/非表示を切り替えます"
    
    group_id: bpy.props.IntProperty(name="Group ID", default=0)
    
    def execute(self, context):
        # グループ内のオブジェクトを取得
        objects = SpawnGroupManager.get_objects_in_group(self.group_id)
        if not objects:
            self.report({'WARNING'}, f"グループ {self.group_id} にオブジェクトが見つかりません")
            return {'CANCELLED'}
        
        # 現在の表示状態を確認（最初のオブジェクトの状態を基準にする）
        current_visibility = not objects[0].hide_viewport
        
        # 表示状態を反転
        SpawnGroupManager.set_group_visibility(self.group_id, not current_visibility)
        
        visibility_text = "非表示" if current_visibility else "表示"
        self.report({'INFO'}, f"グループ {self.group_id} を{visibility_text}にしました")
        
        return {'FINISHED'}


class MYADDON_OT_set_spawn_group(bpy.types.Operator):
    bl_idname = "myaddon.set_spawn_group"
    bl_label = "スポーングループ設定"
    bl_description = "選択したスポーンオブジェクトのグループを設定します"
    
    group_id: bpy.props.IntProperty(name="Group ID", default=0, min=0, max=9)
    
    def execute(self, context):
        selected_objects = [obj for obj in context.selected_objects if "spawn_group" in obj]
        
        if not selected_objects:
            self.report({'WARNING'}, "スポーンオブジェクトが選択されていません")
            return {'CANCELLED'}
        
        # 新しいグループIDを設定
        for obj in selected_objects:
            obj["spawn_group"] = self.group_id
        
        self.report({'INFO'}, f"{len(selected_objects)} 個のオブジェクトをグループ {self.group_id} に設定しました")
        
        return {'FINISHED'}


class MYADDON_OT_show_group_info(bpy.types.Operator):
    bl_idname = "myaddon.show_group_info"
    bl_label = "グループ情報表示"
    bl_description = "現在のグループ情報を表示します"
    
    def execute(self, context):
        groups = SpawnGroupManager.get_existing_groups()
        if not groups:
            self.report({'INFO'}, "スポーングループが見つかりません")
            return {'FINISHED'}
        
        print("=== スポーングループ情報 ===")
        for group_id in groups:
            objects = SpawnGroupManager.get_objects_in_group(group_id)
            visible_count = sum(1 for obj in objects if not obj.hide_viewport)
            total_count = len(objects)
            spawn_time = SpawnGroupManager.get_group_spawn_time(group_id)
            print(f"グループ {group_id}: {visible_count}/{total_count} 個のオブジェクト, スポーン時間: {spawn_time}")
        
        self.report({'INFO'}, f"{len(groups)} 個のグループが存在します（詳細はコンソールを確認）")
        return {'FINISHED'}
    
class MYADDON_OT_adjust_spawn_time(bpy.types.Operator):
    bl_idname = "myaddon.adjust_spawn_time"
    bl_label = "スポーン時間微調整"
    bl_description = "スポーン時間を微調整します"
    
    group_id: bpy.props.IntProperty(name="Group ID", default=0)
    delta: bpy.props.FloatProperty(name="Delta", default=0.1)
    
    def execute(self, context):
        # 現在の値を取得
        current_value = SpawnGroupManager.get_group_spawn_time(self.group_id)
        
        # 新しい値を計算（最小値は0.0）
        new_value = max(0.0, current_value + self.delta)
        
        # 値を設定
        SpawnGroupManager.set_group_spawn_time(self.group_id, new_value)
        
        return {'FINISHED'}


class MYADDON_OT_restore_spawn_properties(bpy.types.Operator):
    bl_idname = "myaddon.restore_spawn_properties"
    bl_label = "スポーンプロパティ復元"
    bl_description = "保存されたスポーンプロパティを復元します"
    
    def execute(self, context):
        SpawnGroupManager.restore_properties()
        self.report({'INFO'}, "スポーンプロパティを復元しました")
        return {'FINISHED'}


# ファイルロード時の処理
@bpy.app.handlers.persistent
def load_post_handler(dummy):
    """ファイルロード後にプロパティを復元"""
    SpawnGroupManager.restore_properties()


# アドオン有効化時の処理
def register():
    # ハンドラーを登録
    if load_post_handler not in bpy.app.handlers.load_post:
        bpy.app.handlers.load_post.append(load_post_handler)


def unregister():
    # ハンドラーを削除
    if load_post_handler in bpy.app.handlers.load_post:
        bpy.app.handlers.load_post.remove(load_post_handler)