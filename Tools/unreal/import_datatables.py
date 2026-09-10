import unreal

ROOT = "/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST"

def import_csv(csv_name, dest_name, struct_path):
    row_struct = unreal.load_object(None, struct_path)
    if row_struct is None:
        unreal.log_error("ROW STRUCT NAO ENCONTRADA: " + struct_path)
        return None
    factory = unreal.CSVImportFactory()
    factory.automated_import_settings.import_row_struct = row_struct
    task = unreal.AssetImportTask()
    task.filename = ROOT + "/Data/" + csv_name
    task.destination_path = "/Game/Data"
    task.destination_name = dest_name
    task.replace_existing = True
    task.automated = True
    task.save = True
    task.factory = factory
    return task

tasks = []
for csv_name, dest, struct in (
    ("DT_Chassis.csv", "DT_Chassis", "/Script/PloidrekRPG.RunnerChassisData"),
    ("DT_Classes.csv", "DT_Classes", "/Script/PloidrekRPG.RunnerClassData"),
):
    t = import_csv(csv_name, dest, struct)
    if t:
        tasks.append(t)

if tasks:
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
    for t in tasks:
        if t.imported_object_paths:
            for p in t.imported_object_paths:
                unreal.log("IMPORTADO OK: " + str(p))
        else:
            unreal.log_error("FALHOU: " + t.destination_name)

# relatorio
for path in ("/Game/Data/DT_Chassis", "/Game/Data/DT_Classes"):
    dt = unreal.load_object(None, path)
    if dt:
        rows = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
        unreal.log("== " + path + " -> " + str(len(rows)) + " linhas: " + ", ".join(str(r) for r in rows))
    else:
        unreal.log_error("nao carregou: " + path)
