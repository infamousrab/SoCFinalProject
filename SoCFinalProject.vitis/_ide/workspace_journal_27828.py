# 2025-04-19T23:04:50.339690400
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

comp = client.get_component(name="Task4App")
status = comp.clean()

platform = client.get_component(name="platform")
status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

client.delete_component(name="Task4App")

comp = client.create_app_component(name="Task4App",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = comp.clean()

vitis.dispose()

