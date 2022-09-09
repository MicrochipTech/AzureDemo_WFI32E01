## Create an Enrollment Group

An enrollment group is an entry for a group of devices that share a common attestation mechanism (e.g. device certificates all derived from a common root and/or signer certificate(s)). Using an enrollment group is recommended for a large number of devices that share an initial configuration and/or for devices that all go to the same tenant. Create an X.509 enrollment group for your IoT Central application so that hundreds, thousands, or even millions/billions of devices can automatically connect to your cloud application without any human intervention.

1. Using the left-hand side navigation pane of your IoT Central application, under `Security` select `Permissions` &gt; `Device connection groups`

   <img src=".//media/image81a.png" style="width:6.5.in;height:3.63506in" />

2. Click on the `+ New` button and create a new enrollment group using any name (with Group type = `IoT devices` and attestation type = `Certificates (X.509)`).  Hit the `Save` icon when finished

   <img src=".//media/image81b.png" style="width:6.5.in;height:3.63506in" />

3. Now that the new enrollment group has been created, click on `Manage Primary`

   <img src=".//media/image82.png" style="width:5.5.in;height:2.53506in" />

4. Click on `+ Add certificate` and browse to the **root** certificate file (which should be located in the `WFI32-IOT` Mass Storage Device). Click the `Upload` button (then click on `Close` when the certificate has been accepted)

   <img src=".//media/image75.png" style="width:5.5.in;height:2.13506in" />

5. Click on `Manage Secondary` and then click on `+ Add certificate`. Browse to the **signer** certificate file (which should be located in the `WFI32-IOT` Mass Storage Device). Click the `Upload` button (then click on `Close` when the certificate has been accepted)

   <img src=".//media/image76.png" style="width:5.5.in;height:2.13506in" />

6. Click on the `Save` icon at the top of the page, and note the ID Scope which was created for the enrollment group. The X.509 enrollment group has been successfully created and should be ready to go!

    <img src=".//media/image83.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />
