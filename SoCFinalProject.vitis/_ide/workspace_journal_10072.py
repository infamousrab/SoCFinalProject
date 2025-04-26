# 2025-04-12T20:05:05.341141900
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

comp = client.get_component(name="HelloWorldApp")
comp.build()

comp = client.get_component(name="Task2App")
comp.build()

comp = client.get_component(name="HelloWorldApp")
comp.build()

comp = client.get_component(name="Task2App")
comp.build()

comp.build()

