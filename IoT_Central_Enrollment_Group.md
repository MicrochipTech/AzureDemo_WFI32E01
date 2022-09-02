# Creating an Enrollment Group for an IoT Central Application

1. Create an X.509 enrollment group for your IoT Central application. If not already opened, launch your IoT Central application and navigate to `Security` in the left-hand side navigation pane and select `Permissions` &gt; `Device connection groups`

   <img src=".//media/image81a.png" style="width:6.5.in;height:3.63506in" />

2. Click on the `+ New` button and create a new enrollment group using any name (Group type = `IoT devices`, attestation type = `Certificates (X.509)`).  Hit the `Save` icon when finished

   <img src=".//media/image81b.png" style="width:6.5.in;height:3.63506in" />

3. Now that the new enrollment group has been created, select `+ Manage Primary`.

   <img src=".//media/image82.png" style="width:5.5.in;height:2.53506in" />

4. Select the file/folder icon associated with the `Primary` field and upload the root certificate file (which should be located in the `WFI32-IOT` Mass Storage Device). The message "`(!) Needs verification`" should appear.  The `Subject` and `Thumbprint` fields will automatically populate themselves

   <img src=".//media/image75.png" style="width:5.5.in;height:3.13506in" />

5. Click `Generate verification code` (this code will be copied to the clipboard which will be needed in a future step)

   <img src=".//media/image76.png" style="width:6.5.in;height:2.03506in" />

6. Create a new temporary folder using any arbitrary name and copy the `root-ca.key` file into the newly-created folder

7. Open a Git Bash window (Start menu &gt; type `Git Bash`)

   <img src=".//media/image15.png" style="width:3.21739in;height:0.94745in" />

8. Using the Git Bash command line, navigate to the newly-created folder 

9. Execute the below command in the Git Bash window (copy and paste for best results)

    **Note**: Once you enter the below command, you will then be asked to enter information for various fields that will be incorporated into your certificate request. Enter the verification code (which was just generated previously) when prompted for the `Common Name`. It's recommended to just copy the `Verification code` to the clipboard and paste it when it's time to enter the `Common Name`.  For the rest of the fields, you can enter anything you want (or just hit `[RETURN]` to keep them blank which is fine for basic demonstration purposes).  If you accidentally hit `[RETURN]` when asked for the `Common Name`, you will need to run the command again...

    ```bash
    openssl req -new -key root-ca.key -out azure_root_ca_verification.csr
    ```

   NOTE: If the `openssl` command does not work, you most likely need to locate the `openssl.exe` program and add its location to your operating system's PATH environment variable

    <img src=".//media/image16.png" style="width:5.in;height:3.18982in" />

10. Generate the verification certificate by executing the following command exactly as shown (suggest copy and paste for best results)

    ```bash
    openssl x509 -req -in azure_root_ca_verification.csr -CA root-ca.crt -CAkey root-ca.key -CAcreateserial -out azure_signer_verification.cer -days 365 -sha256
    ```

11. Click `Verify` and select the `azure_signer_verification.cer` file to upload.  Confirm that the `Primary` certificate has been verified and that a `Thumbprint` has been generated for your certificate.  Click on `Close` to exit the current window, then click on `Save` at the top of the web application window.  The X.509 enrollment group has been successfully created and should be ready to go!

    <img src=".//media/image83.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />
