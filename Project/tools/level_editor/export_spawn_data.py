import bpy
import json
import math
from .enemy_spawn_manager import SpawnGroupManager

class MYADDON_OT_export_spawn_data(bpy.types.Operator):
    bl_idname = "myaddon.export_spawn_data"
    bl_label = "スポーンデータエクスポート"
    bl_description = "スポーンデータをJSONファイルとして出力します"
    
    filepath: bpy.props.StringProperty(subtype="FILE_PATH")
    
    def extract_transform_data(self, obj):
        """export_sceneと同じ方法でトランスフォーム情報を抽出"""
        # ローカルトランスフォーム行列から平行移動、回転、スケーリングを抽出
        trans, rot, scale = obj.matrix_local.decompose()
        
        # QuaternionからEulerに変換
        rot = rot.to_euler()
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)
        
        # トランスフォーム情報をディクショナリに登録
        transform = {
            "translation": (trans.x, trans.y, trans.z),
            "rotation": (rot.x, rot.y, rot.z),
            "scaling": (scale.x, scale.y, scale.z)
        }
        
        return transform
    
    def execute(self, context):
        # スポーンデータを収集
        spawn_data = {
            "groups": [],
            "spawn_points": []
        }
        
        # グループ情報を収集し、spawn_timeをキャッシュ
        groups = SpawnGroupManager.get_existing_groups()
        group_spawn_times = {}
        
        for group_id in groups:
            spawn_time = SpawnGroupManager.get_group_spawn_time(group_id)
            group_data = {
                "id": group_id,
                "spawn_time": spawn_time,
                "object_count": len(SpawnGroupManager.get_objects_in_group(group_id))
            }
            spawn_data["groups"].append(group_data)
            group_spawn_times[group_id] = spawn_time  # キャッシュ
        
        # スポーンポイント情報を収集
        for obj in bpy.data.objects:
            if "spawn_group" in obj:
                group_id = obj["spawn_group"]
                offset = obj.get("spawn_offset", 0.0)
                group_spawn_time = group_spawn_times.get(group_id, 0.0)
                total_spawn_time = group_spawn_time + offset
                
                # export_sceneと同じ方法でトランスフォーム情報を抽出
                transform = self.extract_transform_data(obj)
                
                spawn_point = {
                    "name": obj.name,
                    "group_id": group_id,
                    "spawnTime": total_spawn_time,
                    # C++側のコードに合わせて配列形式で出力
                    "position": [
                        transform["translation"][0],
                        transform["translation"][1], 
                        transform["translation"][2]
                    ],
                    "rotation": [
                        transform["rotation"][0],
                        transform["rotation"][1],
                        transform["rotation"][2]
                    ],
                    "scaling": [
                        transform["scaling"][0],
                        transform["scaling"][1],
                        transform["scaling"][2]
                    ],
                    "transform": transform  # export_sceneと同じ構造も併用
                }
                
                # カスタムプロパティがあれば追加
                if "enemy_type" in obj:
                    spawn_point["enemy_type"] = obj["enemy_type"]
                if "spawn_offset" in obj:
                    spawn_point["spawn_offset"] = offset
                
                spawn_data["spawn_points"].append(spawn_point)
        
        # JSONファイルに出力
        try:
            with open(self.filepath, 'w', encoding='utf-8') as f:
                json.dump(spawn_data, f, indent=2, ensure_ascii=False)
            
            self.report({'INFO'}, f"スポーンデータを {self.filepath} に出力しました")
            return {'FINISHED'}
        except Exception as e:
            self.report({'ERROR'}, f"エクスポートエラー: {str(e)}")
            return {'CANCELLED'}
    
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}