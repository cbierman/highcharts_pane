<?php
/**
 * @file
 * Ctools content type plugin file for individual Highcarts pane.
 */

$plugin = array(
"single" => TRUE, // Just do this one, it is needed.
"title" => t("Highcharts Pane"), // Title to show up on the pane screen.
"description" => t("Custom chart"), // Description to show up on the pane screen.
"category" => t("Charts"), // A category to put this under.
"edit form" => "highcharts_pane_edit_form", // A function that will return the settings form for the pane.
"render callback" => "highcharts_pane_render", // A function that will return the renderable content.
"admin info" => "highcharts_pane_info", // A function that will return the information displayed on the admin screen (optional).
"defaults" => array( // Array of defaults for the settings form.
"text" => "",
),
"all contexts" => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);
 
function highcharts_pane_edit_form($form, &$form_state) {
	$conf = $form_state['conf'];
	$form["#attributes"] = array('enctype' => "multipart/form-data");
	$form["chart_title"] = array(
		'#type' => 'textfield',
	    '#title' => t('Chart Title'),
	    '#description' => t("Title of the chart."),
	    "#default_value" => $conf["chart_title"],
	);

	$form["chart_type"] = array(
		"#type" => "select",
		"#title" => t("Chart Type"),
		"#description" => t("The type of chart to render"),
		"#options" => array(
			"Bar" => "Bar",
			"Line" => "Line",
			"Pie" => "Pie",
		),
		"#default_value" => $conf["chart_type"],
	);

	$form["x_axis"] = array(
		"#type" => "textfield",
		"#title" => t("X-Axis Label"),
		"#description" => t("Label of X-Axis"),
		"#default_value" => $conf["x_axis"],
	);
	$form["y_axis"] = array(
		"#type" => "textfield",
		"#title" => t("Y-Axis Label"),
		"#description" => t("Label of Y-Axis"),
		"#default_value" => $conf["y_axis"],
	);
	$form["csv"] = array(
		"#type" => "file",
		"#title" => t("Choose a file"),
		"#default_value" => '',
		"#size" => 60,
		"#array_parents" => array("csv"),

	);


	return $form;
}

function highcharts_pane_edit_form_validate($form, &$form_state) {
	$destination = file_directory_path() . "/highcharts";
	$file = file_save_upload("csv", array("file_validate_extensions" => array("csv")), $destination);
	// If the file passed validation:
	if (isset($file->filename)) {
	      // Move the file, into the Drupal file system
		  // $newpath = variable_get('file_directory_path');
		if (file_move($file, $file->filename)) {
		// Update the new file location in the database.
	      drupal_write_record('files', $file, 'fid');
	      // Save the file for use in the submit handler.
	      $form_state['storage']['file'] = $file;
	    } else {
	      form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
	    }
	} else {
	    form_set_error('file', t('Invalid file, only images with the extension pdf xls html doc txt are allowed'));
	}
}


function highcharts_pane_edit_form_submit(&$form, &$form_state) {
	dpm($form_state);
	foreach (array("chart_title", "x_axis", "y_axis", "chart_type") as $key) {
		$form_state["conf"][$key] = $form_state["values"][$key];
	}

	$file = $form_state['storage']['file'];
  	// We are done with the file, remove it from storage.
	unset($form_state['storage']['file']);
	// Make the storage of the file permanent
	file_set_status($file, FILE_STATUS_PERMANENT);
	// Set a response to the user.
	drupal_set_message(t('The form has been submitted and the image has been saved, filename: @filename.', array('@filename' => $file->filename)));
}

function highcharts_pane_render($subtype, $conf, $panel_args, $context) {
	
	dpm($conf);


	$my_settings = array(
		"chartID" => strtolower($conf["chart_title"]),
		"chartData" => _get_chart_data($conf),
	);
	dpm($my_settings);

	$block = new stdClass();
	$block->title = $conf["chart_title"];
	$block->content ="<div id='" . strtolower($conf["chart_title"]) . "'></div>";

	libraries_load("highcharts");
	if (($libraries = libraries_load("highcharts")) && ($libraries["loaded"])) {
		drupal_add_js(array("highchartsPane" => $my_settings), "setting");
		drupal_add_js(drupal_get_path("module", "highcharts_pane") . "/js/highcharts_pane.js");
	}

	return $block;
}


function _get_chart_data($options) {
	$chart_data = array(
		"chart" => array(
			"type" => "bar",
			"renderTo" => strtolower($options["chart_title"]),
		),
		"title" => array(
			"text" => $options["chart_title"],
		),
		"xAxis" => array(
			"categories" => array(
				"Apples",
				"Oranges",
				"Potatoes",
			),
		),
		"yAxis" => array(
			"title" => array(
				"text" => $options["y_axis"],
			),
		),
		"series" => array(
			array(
				"name" => "John",
				"data" => array(0, 1, 2),
			),
			array(
				"name" => "Jane",
				"data" => array(5, 7, 3),
			),
		),
	);

	return $chart_data;

}