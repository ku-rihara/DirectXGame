import bpy


# オペレーター：イージングステップを追加
class MYADDON_OT_add_easing_step(bpy.types.Operator):
    bl_idname = "myaddon.add_easing_step"
    bl_label = "イージングステップ追加"
    bl_description = "新しいイージングステップを追加します"
    bl_options = {"REGISTER", "UNDO"}

    group_id: bpy.props.IntProperty()

    def execute(self, context):
        obj = context.object

        step_key = f"easing_group_{self.group_id}_step_count"

        if step_key not in obj:
            obj[step_key] = 0

        step_count = obj[step_key] + 1
        obj[step_key] = step_count

        # 新しいステップのプロパティを初期化
        step_prefix = f"easing_group_{self.group_id}_step_{step_count}"
        obj[f"{step_prefix}_file1"] = ""
        obj[f"{step_prefix}_file2"] = ""
        obj[f"{step_prefix}_file3"] = ""
        obj[f"{step_prefix}_srt1"] = 0  # scale
        obj[f"{step_prefix}_srt2"] = 1  # rotation
        obj[f"{step_prefix}_srt3"] = 2  # translate

        return {"FINISHED"}

# オペレーター：イージングステップを削除
class MYADDON_OT_remove_easing_step(bpy.types.Operator):
    bl_idname = "myaddon.remove_easing_step"
    bl_label = "イージングステップ削除"
    bl_description = "最後のイージングステップを削除します"
    bl_options = {"REGISTER", "UNDO"}

    group_id: bpy.props.IntProperty()

    def execute(self, context):
        obj = context.object

        step_key = f"easing_group_{self.group_id}_step_count"
        if step_key not in obj or obj[step_key] <= 0:
            return {"FINISHED"}

        step_count = obj[step_key]
        step_prefix = f"easing_group_{self.group_id}_step_{step_count}"

        for i in range(1, 4):
            obj.pop(f"{step_prefix}_file{i}", None)
            obj.pop(f"{step_prefix}_srt{i}", None)

        obj[step_key] = step_count - 1

        return {"FINISHED"}


