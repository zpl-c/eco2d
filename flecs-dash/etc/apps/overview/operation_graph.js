const operation_chart = {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        {
          label: 'New',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#5BE595',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "count"
        },
        {
          label: 'Delete',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#46D9E6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "count"
        },
        {
          label: 'Set',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#2D5BE6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "count"
        },      
        {
          label: 'Add',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#6146E6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "count"
        },
        {
          label: 'Remove',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#E550E6',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "count"
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
          id: 'count',
          ticks: {
            beginAtZero: true,
            padding: 10,
            callback: function(value, index, values) {
                return value + "/s";
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
  
  Vue.component('operation-graph', {
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
  
        operation_chart.data.labels = labels;
        operation_chart.data.datasets[0].data = this.data.world.history_1m.new_count.avg;
        operation_chart.data.datasets[1].data = this.data.world.history_1m.delete_count.avg;
        operation_chart.data.datasets[2].data = this.data.world.history_1m.set_count.avg;
        operation_chart.data.datasets[3].data = this.data.world.history_1m.add_count.avg;
        operation_chart.data.datasets[4].data = this.data.world.history_1m.remove_count.avg;
      },
      createChart() {
        const ctx = document.getElementById('operation-graph');
  
        this.chart = new Chart(ctx, {
          type: operation_chart.type,
          data: operation_chart.data,
          options: operation_chart.options
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
        <canvas id="operation-graph" :data-fps="tick"></canvas>
      </div>`
  });
