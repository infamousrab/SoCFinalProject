# 2025-04-18T13:43:20.779070300
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

platform = client.get_component(name="platform")
status = platform.build()

status = platform.build()

comp = client.get_component(name="Task2App")
comp.build()

status = platform.build()

comp = client.get_component(name="Task3App")
comp.build()

status = platform.build()

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

vitis.dispose()

