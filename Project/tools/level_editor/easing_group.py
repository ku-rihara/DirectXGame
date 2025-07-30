import bpy
# オペレーター：イージンググループを追加
class MYADDON_OT_add_easing_group(bpy.types.Operator):
    bl_idname = "myaddon.add_easing_group"
    bl_label = "イージンググループ追加"
    bl_description = "新しいイージンググループを追加します"
    bl_options = {"REGISTER", "UNDO"}

    def execute(self, context):
        obj = context.object

        # イージンググループの数を取得または初期化
        if "easing_group_count" not in obj:
            obj["easing_group_count"] = 0

        group_count = obj["easing_group_count"]
        obj["easing_group_count"] = group_count + 1

        # 新しいグループのステップカウンターを初期化
        obj[f"easing_group_{group_count}_step_count"] = 0
        
        # 新しいグループの開始時間を初期化
        obj[f"easing_group_{group_count}_start_time"] = 0.0

        return {"FINISHED"}

# オペレーター：イージンググループを削除
class MYADDON_OT_remove_easing_group(bpy.types.Operator):
    bl_idname = "myaddon.remove_easing_group"
    bl_label = "イージンググループ削除"
    bl_description = "最後のイージンググループを削除します"
    bl_options = {"REGISTER", "UNDO"}

    def execute(self, context):
        obj = context.object

        if "easing_group_count" not in obj or obj["easing_group_count"] <= 0:
            return {"FINISHED"}

        group_count = obj["easing_group_count"]
        last_group = group_count - 1

        # 最後のグループのすべてのプロパティを削除
        self.remove_group_properties(obj, last_group)

        obj["easing_group_count"] = group_count - 1

        return {"FINISHED"}

    def remove_group_properties(self, obj, group_id):
        """指定されたグループのすべてのプロパティを削除"""
        step_count_prop = f"easing_group_{group_id}_step_count"
        start_time_prop = f"easing_group_{group_id}_start_time"

        if step_count_prop in obj:
            step_count = obj[step_count_prop]

            # 各ステップのプロパティを削除
            for step in range(1, step_count + 1):
                for i in range(1, 4):
                    file_prop = f"easing_group_{group_id}_step_{step}_file{i}"
                    srt_prop = f"easing_group_{group_id}_step_{step}_srt{i}"

                    if file_prop in obj:
                        del obj[file_prop]
                    if srt_prop in obj:
                        del obj[srt_prop]

            # ステップカウンターを削除
            del obj[step_count_prop]
        
        # 開始時間プロパティを削除
        if start_time_prop in obj:
            del obj[start_time_prop]