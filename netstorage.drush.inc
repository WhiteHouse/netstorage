<?php

/**
 * @file
 * Drush commands for the NetStorage module.
 */

/**
 * Implements hook_drush_command().
 */
function netstorage_drush_command() {
  $items = array();

  $items['netstorage-get-scp-command'] = array(
    'description' => "Prints a command to manually rsync a single file to Akamai's NetStorage.",
    'arguments' => array(
      'filepath' => 'path/to/file to be synced',
    ),
    'options' => array(
      'subdir' => 'Subdirectory on NetStorage to push file into' 
    ),
    'examples' => array(
      'drush netstorage-get-scp-command "something.pdf"' => 'If you set %netstorage_upload_path to "htdocs/sites/default/files" in settings.php this command will push your file up to htdocs/sites/default/files/something.pdf on NetStorage.',
      'drush ngscp "/local/path/to/something.pdf --subdir=example"' => 'Gets the command to update %netstorage_upload_path/example/something.pdf on NetStorage.',
    ),
    'aliases' => array('nscp'),
  );

  $items['netstorage-set-test-destination'] = array(
    'description' => "Overrides SCP destination based on netstorage_credentials in settings.php for testing. NOTE: This assumes you to NOT need to use a key file to verify your identity (scp -i ). (This is set here as a variable, not passed somewhere as an option so that the command can still be executed by processing the netstorage_upload queue using drush queue-run.)",
    'arguments' => array(
      'destination' => 'username@host.com:/path/to/destination/directory',
    ),
    'examples' => array(
      'drush netstorage-set-test-destination "example@myserver.com:/home/example"' => '',
      'drush nstd "example@myserver.com:/home/example"' => '',
    ),
    'aliases' => array('nstd'),
  );

  $items['netstorage-unset-test-destination'] = array(
    'description' => "Unset test SCP destination.",
    'examples' => array(
      'drush netstorage-unset-test-destination' => '',
      'drush nutd' => '',
    ),
    'aliases' => array('nutd'),
  );

  $items['netstorage-queue-run'] = array(
    'description' => "Wrapper around drush queue-run to add delay.",
    'arguments' => array(
      'queue_name' => 'Presumably you would use "netstorage_upload", but you can technically use this for any queue.',
    ),
    'options' => array(
      'delay' => 'Seconds to wait before executing command. (Handy for staggering multiple cron jobs all running in the same minute.)',
      'path-to-drush' => 'If you have different versions of drush on your machine (e.g. Acquia Cloud) you can specify the path to drush here.',
    ),
    'examples' => array(
      'drush netstorage-queue-run netstorage_upload --delay=15' => 'Wait 15 seconds, then execute this command.',
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function netstorage_drush_help($section) {
  switch ($section) {
    case 'drush:netstorage-get-scp-command':
      return dt("Pass path/to/some/file.example to this script. It will return a command to manually sync this file to NetStorage.");
  }
}

/**
 * Implements drush_hook_COMMAND().
 *
 * @see netstorage_drush_command()
 */
function drush_netstorage_get_scp_command($file) {
  // Get absolute path to file.
  $file = (substr($file, 0, 1) == '/') ? $file : $_ENV['PWD'] . '/' . $file;
  if (!file_exists($file)) {
    drush_print('Sorry. This file does not exist: ' . $file);
    return;
  }
  else {
    $file = realpath($file);
  }

  // Get subdir (if one is provided).
  $subdir = drush_get_option('subdir', '');

  $command = netstorage_get_scp_command($file, $subdir);
  drush_print($command);
}

/**
 * Callback for netstorage-queue-run. Wrapper around drush queue-run.
 */
function drush_netstorage_queue_run($queue_name) {
  if ($seconds = drush_get_option('delay', 0)) {
    // Delay is used to stagger multiple drush commands running on cron in the same minute.  
    watchdog('drush netstorage-queue-run', "Wait {$seconds} seconds");
    sleep($seconds);
  }

  // Sepecify which drush to use. User can optionally give path to drush on server.
  $drush = drush_get_option('path-to-drush', 'drush');

  // Execute the command.
  $command = "{$drush} queue-run {$queue_name}";
  watchdog('drush netstorage-queue-run', "Running: {$command}");
  shell_exec($command);
}

/**
 * Set test SCP destination.
 */
function drush_netstorage_set_test_destination($destination) {
  variable_set('netstorage_test_destination', $destination);
}

/**
 * Unset test SCP destination.
 */
function drush_netstorage_unset_test_destination() {
  variable_set('netstorage_test_destination', '');
}
