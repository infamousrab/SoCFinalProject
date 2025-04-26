# 2025-04-23T20:15:19.853859700
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.get_component(name="Task4App")
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

status = platform.build()

comp.build()

status = platform.build()

comp.build()

comp = client.get_component(name="HelloWorldApp")
status = comp.clean()

status = platform.build()

comp.build()

comp = client.get_component(name="Task2App")
status = comp.clean()

status = platform.build()

comp.build()

status = platform.build()

comp = client.get_component(name="Task3App")
comp.build()

status = platform.build()

comp = client.get_component(name="Task4App")
comp.build()

vitis.dispose()

