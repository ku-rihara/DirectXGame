import bpy
import math
import bpy_extras
import json


class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_idname="myaddon.myaddon_ot_export_scene"
    bl_label="シーン出力"
    bl_description="シーン情報をExportします"
    #出力するファイルの拡張子
    filename_ext=".json"

    def export_json(self):
        """JSON形式でファイルに出力"""

        #保存する情報をまとめるdict
        json_object_root=dict()

        #ノード名
        json_object_root["name"]="scene"
        #オブジェクトリストを作成
        json_object_root["objects"]=list()

        #Todo: シーン内の全オブジェクト走査をしてパック
        for object in bpy.context.scene.objects:
            #親オブジェクトがあるものはスキップ
            if(object.parent):
                continue
            #シーン直下のオブジェクトをルートノード(深さ0)とし、再帰関数で走査
            self.parse_scene_recursive_json(json_object_root["objects"],object,0)

        #オヴジェクトをJSON文字列にエンコード
        json_text =json.dumps(json_object_root,ensure_ascii=False,cls=json.JSONEncoder,indent=4)
        #コンソールに表示してみる
        print(json_text)

        #ファイルをテキスト形式で書き出し用にオープン
        #スコープを抜けると自動的にクローズされる
        with open(self.filepath,"wt",encoding="utf-8")as file:
            #ファイルに文字列を書き込む
            file.write(json_text)

    def parse_scene_recursive_json(self,data_parent,object,level):

        #シーンのオブジェクト1個分のjsonオブジェクトを生成
        json_object=dict()
        #オブジェクトの種類
        json_object["type"]=object.type
        #オブジェクト名
        json_object["name"]=object.name

        #Todo: その他情報をパック
        trans,rot,scale=object.matrix_local.decompose()
        #QuaternionからEuler
        rot=rot.to_euler()
        rot.x=math.degrees(rot.x)
        rot.y=math.degrees(rot.y)
        rot.z=math.degrees(rot.z)
        #トランスフォーム情報をディクショナリに登録
        transform=dict()
        transform["translation"]=(trans.x,trans.y,trans.z)
        transform["rotation"]=(rot.x,rot.y,rot.z)
        transform["scaling"]=(scale.x,scale.y,scale.z)
        #まとめて1個分のjsonオブジェクトに登録
        json_object["transform"]=transform

        if "isDisable" in object:
            json_object["isDisable"]=object["isDisable"]

        #カスタムプロパティ'filename'
        if "file_name" in object:
            json_object["file_name"]=object["file_name"]

        #カスタムプロパティ'collider'
        if "collider" in object:
            collider=dict()
            collider["type"]=object["collider"]
            collider["center"]=object["collider_center"].to_list()
            collider["size"]=object["collider_size"].to_list()
            json_object["collider"]=collider

        #カスタムプロパティ'emitter'
        if hasattr(object, 'emitter_list') and len(object.emitter_list) > 0:
            emitters_list = []
            for emitter in object.emitter_list:
                emitter_data = {
                    "particle_name": emitter.particle_name,
                    "primitive_type": emitter.primitive_type
                }
                emitters_list.append(emitter_data)
            json_object["emitters"] = emitters_list
          
        # Easing groups処理（安全な実装）
        try:
            group_ids = []

            # easing_group_X_step_count が存在するグループID一覧を収集
            for key in object.keys():
                if key.startswith("easing_group_") and key.endswith("_step_count"):
                    try:
                        # 数字部分を取り出す
                        parts = key.split("_")
                        if len(parts) >= 3:  # 配列の境界チェック
                            group_id = int(parts[2])
                            group_ids.append(group_id)
                    except (ValueError, IndexError):
                        # エラーが発生した場合はスキップ
                        continue

            if group_ids:  # group_idsが空でない場合のみ処理
                easing_groups = []

                for group_id in group_ids:
                    step_count = object.get(f"easing_group_{group_id}_step_count", 0)
                    if step_count == 0:
                        continue

                    group_data = {
                        "group_id": group_id,
                        "steps": []
                    }

                    # 開始時間を安全に取得
                    start_time_key = f"easing_group_{group_id}_start_time"
                    if start_time_key in object:
                        # プロパティの値を安全に取得
                        start_time_value = object[start_time_key]
                        # 数値型かチェック
                        if isinstance(start_time_value, (int, float)):
                            group_data["start_time"] = float(start_time_value)
                        else:
                            # 文字列の場合は変換を試行
                            try:
                                group_data["start_time"] = float(start_time_value)
                            except (ValueError, TypeError):
                                group_data["start_time"] = 0.0  # デフォルト値
                    else:
                        group_data["start_time"] = 0.0  # デフォルト値

                    # ステップ処理
                    for step in range(1, step_count + 1):
                        step_data = {
                            "step_number": step,
                            "files": []
                        }

                        for i in range(1, 4):
                            file_key = f"easing_group_{group_id}_step_{step}_file{i}"
                            srt_key = f"easing_group_{group_id}_step_{step}_srt{i}"

                            filename = object.get(file_key, "")
                            
                            if srt_key not in object:
                                continue
                            
                            srt_index = object[srt_key]
                            srt_names = ["Scale", "Rotation", "Transform"]

                            # インデックスが有効範囲内かチェック
                            if 0 <= srt_index < len(srt_names) and filename and filename.strip():
                                file_data = {
                                    "file_index": i,
                                    "filename": filename,
                                    "srt_type": srt_names[srt_index]
                                }
                                step_data["files"].append(file_data)

                        if step_data["files"]:
                            group_data["steps"].append(step_data)

                    if group_data["steps"]:
                        easing_groups.append(group_data)

                if easing_groups:
                    json_object["easing_groups"] = easing_groups

        except Exception as e:
            # エラーが発生した場合はログに出力してスキップ
            print(f"Error processing easing groups for object {object.name}: {e}")

        #1こ分のjsonオブジェクトを親オブジェクトに登録
        data_parent.append(json_object)

        #Todo:直接の子供リストを走査
        if len(object.children)>0:
            #子ノードリストを作成
            json_object["children"]=list()

            #子ノードへ進
            for child in object.children:
                self.parse_scene_recursive_json(json_object["children"],child,level+1)


    def export(self):
        print("シーン情報出力開始...%r" % self.filepath)

        with open(self.filepath,"wt") as file:

            file.write("SCENE")

             #シーン内の全オブジェクトについて
            for object in bpy.context.scene.objects:
               
                if(object.parent):
                   continue
               
               #シーン直下のオブジェクトルートノード
                self.parse_scene_recursive(file,object,0)

    def execute(self,context):
        print("シーン情報をExportします")

        #ファイルに出力
        self.export_json()

        self.report({'INFO'},"シーン情報をExportしました")
        print("シーン情報をExportしました")

        return {'FINISHED'}

    def write_and_print(self,file,str):
        print(str)

        file.write(str)
        file.write('\n')

    def parse_scene_recursive(self,file,object,level):

        #深さ分がインデントする
        indent =''
        for i in range(level):
            indent+="\t"            

        #オブジェクト名書き込み
        self.write_and_print(file,indent + object.type)
         #ローカルトランスフォーム行列から平行移動、回転、スケーリングを抽出
        trans,rot,scale=object.matrix_local.decompose()
        #回転をQuaternionからEluerに変換
        rot=rot.to_euler()
        rot.x=math.degrees(rot.x)
        rot.y=math.degrees(rot.y)
        rot.z=math.degrees(rot.z)
        #トランスフォーム情報を表示
        self.write_and_print(file,indent + "Trans(%f,%f,%f)"%(trans.x,trans.y,trans.z))
        self.write_and_print(file,indent + "Rot(%f,%f,%f)"%(rot.x,rot.y,rot.z))
        self.write_and_print(file,indent + "Scale(%f,%f,%f)"%(scale.x,scale.y,scale.z))
        if "file_name" in object:
            self.write_and_print(file,indent + "N %s" % object["file_name"])
            #カスタムプロパティ'collider'
        if "collider"in object:
            self.write_and_print(file,indent + "C %s" % object["collider"])
            temp_str=indent + "CC %f %f %f"
            temp_str %= (object["collider_center"][0],object["collider_center"][1],object["collider_center"][2])
            self.write_and_print(file,temp_str)
            temp_str= indent + "CS %f %f %f"
            temp_str%= (object["collider_size"][0],object["collider_size"][1],object["collider_size"][2])
            self.write_and_print(file,temp_str)
        self.write_and_print(file,indent + 'END')
        self.write_and_print(file,'')

        #子ノードへと進
        for child in object.children:
            self.parse_scene_recursive(file,child,level+1)