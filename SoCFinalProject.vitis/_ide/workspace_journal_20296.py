# 2025-04-12T16:06:59.517113300
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

status = client.add_platform_repos(platform=["/c:/SoCFinalProject/SOCFinalProject.vitis/platform/export/platform"])

comp = client.create_app_component(name="SoCFinalProjectApplication",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

comp = client.get_component(name="SoCFinalProjectApplication")
comp.build()

comp.build()

comp.build()

comp.build()

client.delete_component(name="SoCFinalProjectApplication")

status = client.add_platform_repos(platform=["/c:/SoCFinalProject/SOCFinalProject.vitis/platform/export/platform"])

comp = client.create_app_component(name="HelloWorldApp",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

comp = client.get_component(name="HelloWorldApp")
comp.build()

status = client.add_platform_repos(platform=["/c:/SoCFinalProject/SOCFinalProject.vitis/platform/export/platform"])

comp = client.create_app_component(name="Task2App",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_microblaze_riscv_0")

client.delete_component(name="Task2App")

status = client.add_platform_repos(platform=["/c:/SoCFinalProject/SOCFinalProject.vitis/platform/export/platform"])

comp = client.create_app_component(name="Task2App",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

comp = client.get_component(name="Task2App")
comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

vitis.dispose()

