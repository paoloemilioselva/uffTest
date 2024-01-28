uffTest
-------

This is a simple example of a (u)sd (f)ile (f)ormat plugin, which allows you to procedurally generate usd-primitives.

The prim needs to be defined with a payload, which could be just an anon layer, as it doesn't need any file, it will just produce its data procedurally.

    #usda 1.0
    def "uffItem" (
        uffParams = {
            float randomRotY = 0.0
	          int cubesX = 1
	          int cubesY = 1
	          int cubesZ = 1
        }
        payload = @anon:dummy:myproc.uff@</uff>
    )
    {
	      float randomRotY = 0.0
	      int cubesX = 1
	      int cubesY = 1
	      int cubesZ = 1
    }

You can see that procedural parameters can be defined as metadata or as attributes on the prim.
The latter is preferred, as it could be timeSampled if needed.
You don't need both, here it is just an example showing how to write them.
In this example plugin, attributes will always override metadata params.

You can change how many cubes you want on each axis and how much random-rotation-Y degrees you want per cube.

Benefits of fileformat plugins is that they generate usd-prims, so you could use `usdcat` or a Solaris USD_ROP flattened to just bake the resulting prims in a fully portable usd file.

The plugin lets you compile for standard OpenUSD or for SideFX Solaris, installing directly in the right Documents folder to be picked up by Solaris (please, modify installation paths if required).



https://github.com/paoloemilioselva/uffTest/assets/1212607/cb889844-2ae8-4101-af84-900c0f31b2cc


