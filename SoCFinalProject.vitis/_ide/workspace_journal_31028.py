# 2025-04-19T21:59:53.937241300
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

client.delete_component(name="xintc_example")

client.delete_component(name="xintc_low_level_example")

client.delete_component(name="xintc_tapp_example")

comp = client.get_component(name="HelloWorldApp")
status = comp.clean()

comp = client.get_component(name="Task2App")
status = comp.clean()

comp = client.get_component(name="Task3App")
status = comp.clean()

platform = client.get_component(name="platform")
status = platform.build()

comp = client.get_component(name="HelloWorldApp")
comp.build()

status = platform.build()

comp = client.get_component(name="Task2App")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp = client.get_component(name="Task3App")
comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

comp = client.create_app_component(name="Task4App",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

status = platform.build()

comp = client.get_component(name="Task4App")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()

