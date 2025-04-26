# 2025-04-18T16:05:35.549356900
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.get_component(name="Task3App")
comp.build()

status = comp.clean()

status = platform.build()

comp.build()

vitis.dispose()

