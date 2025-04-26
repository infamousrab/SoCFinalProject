# 2025-04-12T19:49:37.805348100
import vitis

client = vitis.create_client()
client.set_workspace(path="SoCFinalProject.vitis")

comp = client.get_component(name="Task2App")
comp.build()

vitis.dispose()

