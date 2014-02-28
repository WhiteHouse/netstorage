NetStorage
==========

This module helps you sync files up to Akamai NetStorage.

Store your private NetStorage credentials in settings.php like this:

  ```php
  $conf['netstorage_rsync_variables'] = array(
    '%key_file' => "/private/path/to/netstorage.key",
    '%user' => 'myUserName',
    '%customer' => 'myCustomer',
    '%cp_code' => 'CPCodeProvidedByAkamai',
    '%netstorage_upload_path' => 'htdocs/path/to/your/files/directory'
  );
  ```

If you have tweetfetch (included in tweetserver profile) installed, this module queues jobs for pushing static json tweet files up to netstorage. Process the queue with `drush queue-run netstorage_upload`.
