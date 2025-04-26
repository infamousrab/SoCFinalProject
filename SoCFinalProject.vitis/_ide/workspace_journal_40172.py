# 2025-04-12T16:00:10.724498100
import vitis

client = vitis.create_client()
client.set_workspace(path="SOCFinalProject.vitis")

platform = client.create_platform_component(name = "platform",hw_design = "$COMPONENT_LOCATION/../../design_1_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

platform = client.get_component(name="platform")
status = platform.build()

vitis.dispose()

