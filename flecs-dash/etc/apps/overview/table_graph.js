
const table_chart = {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        {
          label: 'Total',
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
          label: 'Matched',
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
          label: 'Singleton',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
              '#4596E5',
          ],
          borderWidth: 2,
          pointRadius: 0,
          yAxisID: "count"
        },
        {
          label: 'Empty',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
              '#2D5BE6',
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
      legend: {
        display: true
      },
      scales: {
        yAxes: [{
          id: 'count',
          ticks: {
            beginAtZero: true,
            padding: 10,
            callback: function(value, index, values) {
                return value;
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
  
  Vue.component('table-graph', {
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
        var length = this.data.world.history_1m.table_count.avg.length;
        for (var i = 0; i < length; i ++) {
            labels.push((length  - i) + "s");
        }
  
        table_chart.data.labels = labels;
        table_chart.data.datasets[0].data = this.data.world.history_1m.table_count.avg;
        table_chart.data.datasets[1].data = this.data.world.history_1m.matched_table_count.avg;
        table_chart.data.datasets[2].data = this.data.world.history_1m.singleton_table_count.avg;
        table_chart.data.datasets[3].data = this.data.world.history_1m.empty_table_count.avg;
      },
      createChart() {
        const ctx = document.getElementById('table-graph');
        this.chart = new Chart(ctx, {
          type: table_chart.type,
          data: table_chart.data,
          options: table_chart.options
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
        <canvas id="table-graph" :data-fps="tick"></canvas>
      </div>`
  });
