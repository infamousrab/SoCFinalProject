# 2025-04-13T02:29:43.190689100
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

platform = client.get_component(name="platform")
status = platform.build()

status = platform.build()

comp = client.get_component(name="Task3App")
comp.build()

vitis.dispose()

