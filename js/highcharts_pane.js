/**
 * @file
 * Javascript for the highcharts pane module.
 */

 // Namespace jQuery to avoid conflicts.
(function($) {
  	//Define the behavior.
  	Drupal.behaviors.highchartsPane = {
  		attach: function(context, settings) {
  			var chartID = "#" + Drupal.settings.highchartsPane.chartID;
        console.log(Drupal.settings.highchartsPane);
        $(chartID, context).once("highchartsPane", function() {
          var chart = new Highcharts.Chart(Drupal.settings.highchartsPane.chartData);
          console.log(chart);
  			});
  		}
  	};
})(jQuery);