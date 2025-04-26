# 2025-04-12T20:25:12.239647100
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

comp = client.get_component(name="Task2App")
comp.build()

platform = client.get_component(name="platform")
status = platform.build()

comp.build()

status = platform.build()

comp.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

comp.build()

comp.build()

status = platform.build()

vitis.dispose()

