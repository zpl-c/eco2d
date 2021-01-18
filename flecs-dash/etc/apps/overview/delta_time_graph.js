const delta_time_chart = {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        {
          label: 'avg',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#5BE595',
          ],
          borderWidth: 2,
          pointRadius: 0
        },
        {
          label: 'min',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#40805B',
          ],
          borderWidth: 1,
          pointRadius: 0
        },
        {
          label: 'max',
          data: [],
          backgroundColor: [ 'rgba(0,0,0,0)' ],
          borderColor: [
            '#40805B',
          ],
          borderWidth: 1,
          pointRadius: 0
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
          id: 'time',
          position: 'right',
          ticks: {
            beginAtZero: false,
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
  
  Vue.component('delta-time-graph', {
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
  
        delta_time_chart.data.labels = labels;
        delta_time_chart.data.datasets[0].data = this.data.world.history_1m.delta_time.avg;
        delta_time_chart.data.datasets[1].data = this.data.world.history_1m.delta_time.min;
        delta_time_chart.data.datasets[2].data = this.data.world.history_1m.delta_time.max;
      },
      createChart() {
        const ctx = document.getElementById('delta-time-graph');
        this.chart = new Chart(ctx, {
          type: delta_time_chart.type,
          data: delta_time_chart.data,
          options: delta_time_chart.options
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
        <canvas id="delta-time-graph" :data-fps="tick"></canvas>
      </div>`
  });
