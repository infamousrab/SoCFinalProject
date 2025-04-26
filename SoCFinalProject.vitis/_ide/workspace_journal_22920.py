# 2025-04-12T21:17:08.709745900
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.get_component(name="Task2App")
comp.build()

status = platform.build()

client.delete_component(name="platform")

platform = client.create_platform_component(name = "platform",hw_design = "$COMPONENT_LOCATION/../../design_1_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

status = platform.build()

status = platform.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

client.delete_component(name="platform")

platform = client.create_platform_component(name = "platform",hw_design = "$COMPONENT_LOCATION/../../SoCFinalProject.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

status = platform.build()

status = platform.build()

comp = client.get_component(name="HelloWorldApp")
comp.build()

status = platform.build()

vitis.dispose()

