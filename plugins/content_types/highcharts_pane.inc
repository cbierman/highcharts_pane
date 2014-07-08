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

	form_load_include($form_state, "inc", "highcharts_pane", "plugins/content_types/highcharts_pane");
	$conf = $form_state['conf'];
	dpm($conf);
	$form["chart_title"] = array(
		'#type' => 'textfield',
	    '#title' => t('Chart Title'),
	    '#description' => t("Title of the chart."),
	    "#default_value" => isset($conf["chart_title"]) ? $conf["chart_title"] : '',
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
		"#default_value" => isset($conf["chart_type"]) ? $conf["chart_type"] : '',
	);

	$form["x_axis"] = array(
		"#type" => "textfield",
		"#title" => t("X-Axis Label"),
		"#description" => t("Label of X-Axis"),
		"#default_value" => isset($conf["x_axis"]) ? $conf["x_axis"] : '',
	);
	$form["y_axis"] = array(
		"#type" => "textfield",
		"#title" => t("Y-Axis Label"),
		"#description" => t("Label of Y-Axis"),
		"#default_value" => isset($conf["y_axis"]) ? $conf["y_axis"] : '',
	);
	$form["csv"] = array(
		"#type" => "managed_file",
		"#default_value" => isset($conf["csv"]) ? $conf["csv"] : '',
		"#title" => t("CSV"),
		"#upload_location" => "public://highcharts",
		"#upload_validators" => array(
			"file_validate_extensions" => array("csv"),
		),

	);

	return $form;
}

// function highcharts_pane_csv_process($element, &$form_state, $form) {
//   $element = file_managed_file_process($element, $form_state, $form);
//   $element['upload_button']['#access'] = FALSE;
//   return $element;
// }

function highcharts_pane_edit_form_submit(&$form, &$form_state) {

	foreach (array("chart_title", "x_axis", "y_axis", "chart_type") as $key) {
		$form_state["conf"][$key] = $form_state["values"][$key];
	}

	if ($form_state["values"]["csv"] != 0) {

		// Load the file via file.fid.
		$file = file_load($form_state['values']['csv']);
		// Change status to permanent.
		$file->status = FILE_STATUS_PERMANENT;
		// Save.
		file_save($file);
		// Record that the module is using the file. 
		file_usage_add($file, 'highcharts_pane', 'user', $file->uid);
		$form_state["conf"]["csv"] = $form_state["values"]["csv"];
		drupal_set_message(t("The file @file_name was uploaded", array("@file_name" => $file->filename,)), 'status', FALSE);
	}
	else if ($form_state["values"]["csv"] == 0) {
		$fid = $form_state["conf"]["csv"];
		$file = $fid ? file_load($fid) : FALSE;
		if ($file) {
			file_usage_delete($file, "highcharts_pane", 'user', $file->uid, 1);

			file_delete($file);
		}
		$form_state["conf"]["csv"] = FALSE;
		drupal_set_message(t("The file @file_name was removed", array("@file_name" => $file->filename)), 'status', FALSE);
	}

}

function highcharts_pane_render($subtype, $conf, $panel_args, $context) {
	
	dpm($conf);

	if (!isset($conf["chart_id"])) {
		$chartID = strtolower($conf["chart_title"]);
		$chartID = preg_replace("/[\s-]+/", " ", $chartID);
	    //Convert whitespaces and underscore to dash
	    $chartID = preg_replace("/[\s_]/", "-", $chartID);

	    $conf["chart_id"] = $chartID;
	} else {
		$chartID = $conf["chart_id"];
	}

	$highcharts_settings = array(
		"chartID" => $chartID,
		"chartData" => _get_chart_data($conf),
	);

	$block = new stdClass();
	$block->title = $conf["chart_title"];
	$block->content ="<div id='" . $chartID . "'></div>";

	libraries_load("highcharts");
	if (($libraries = libraries_load("highcharts")) && ($libraries["loaded"])) {
		drupal_add_js(array("highchartsPane" => $highcharts_settings), "setting");
		drupal_add_js(drupal_get_path("module", "highcharts_pane") . "/js/highcharts_pane.js");
	}

	return $block;
}


function _get_chart_data($options) {
	$chart_data = array(
		"chart" => array(
			"type" => "bar",
			"renderTo" => $options["chart_id"],
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