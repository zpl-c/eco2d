const performance_chart = {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        {
          label: 'FPS',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#5BE595',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "fps"
        },
        {
          label: 'Frame',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#6146E6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "time"
        },      
        {
          label: 'Systems',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#46D9E6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "time"
        },
        {
          label: 'Merging',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#E550E6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "time"
        }
      ]
    },
    options: {
      title: {
        display: false
      },
      responsive: true,
      maintainAspectRatio: false,
      lineTension: 1,
      animation: {
        duration: 0
      },    
      scales: {
        yAxes: [{
          id: 'fps',
          ticks: {
            beginAtZero: true,
            padding: 10,
            callback: function(value, index, values) {
                return value + "Hz";
            }
          }
        }, {
          id: 'time',
          position: 'right',
          ticks: {
            beginAtZero: true,
            padding: 10,
            callback: function(value, index, values) {
                return (1000 * value).toFixed(2) + "ms";
            }
          }
        }],
        xAxes: [{
          ticks: {
            minRotation: 0,
            maxRotation: 0,
            maxTicksLimit: 20
          }
        }]
      }
    }  
  }
  
  Vue.component('performance-graph', {
    props: ['tick', 'data'],
    updated() {
      this.updateChart();
    },
    data: function() {
      return {
        chart: undefined
      }
    },
    methods: {
      setValues() {
        if (!this.data.world) {
          return;
        }
  
        if (!this.chart) {
          this.createChart();
        }
  
        var labels = [];
        var length = this.data.world.history_1m.frame_time_total.avg.length;
        for (var i = 0; i < length; i ++) {
            labels.push((length  - i) + "s");
        }
  
        performance_chart.data.labels = labels;
        performance_chart.data.datasets[0].data = this.data.world.history_1m.fps.avg;
        performance_chart.data.datasets[1].data = this.data.world.history_1m.frame_time_total.avg;
        performance_chart.data.datasets[2].data = this.data.world.history_1m.system_time_total.avg;
        performance_chart.data.datasets[3].data = this.data.world.history_1m.merge_time_total.avg;
      },
      createChart() {
        const ctx = document.getElementById('fps-graph');
  
        performance_chart.options.scales.yAxes[1].ticks.suggestedMax = 1.0 / this.data.target_fps;
  
        this.chart = new Chart(ctx, {
          type: performance_chart.type,
          data: performance_chart.data,
          options: performance_chart.options
        });
      },
      updateChart() {
        if (!this.data.world) {
          return;
        }
        this.setValues();
        this.chart.update();
      }
    },
    template: `
      <div class="app-graph">
        <canvas id="fps-graph" :data-fps="tick"></canvas>
      </div>`
  });