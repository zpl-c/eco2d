
const system_time_chart = {
  type: 'line',
  data: {
    labels: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
              11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
              21, 22, 23, 24, 12, 26, 27, 28, 29, 30,
              31, 32, 33, 34, 13, 36, 37, 38, 39, 40,
              41, 42, 43, 44, 14, 46, 47, 48, 49, 50,
              51, 52, 53, 54, 15, 56, 57, 58, 59, 60],
    datasets: [
      {
        label: 'Matched entities',
        data: [],
        backgroundColor: [ '#26537F' ],
        borderColor: [
          '#4596E5',
        ],
        borderWidth: 2,
        pointRadius: 0,
        yAxisID: "entities"
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
    hover: {
      animationDuration: 0 // duration of animations when hovering an item
    },
    responsiveAnimationDuration: 0,
    legend: {
      display: false
    },
    elements: {
      line: {
          tension: 0 // disables bezier curves
      }
    },    
    scales: {
      yAxes: [{
        id: 'entities',
        position: 'right',
        gridLines: {
          display:false
        }, 
        ticks: {     
          display: false,
          beginAtZero: true,
          maxTicksLimit: 2
        }
      }],
      xAxes: [{
        gridLines: {
          display:false
        },        
        ticks: {
          display: false
        }
      }]
    }
  }  
}

Vue.component('system-time-graph', {
  props: ['tick', 'system', 'system_time', 'percentage'],
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
      if (!this.system.history_1m) {
        return;
      }

      if (!this.chart) {
        this.createChart();
      }

      system_time_chart.data.datasets[0].data = this.system.history_1m.time_spent.avg;
    },

    createChart() {
      const ctx = document.getElementById('system-time-' + this.system.name + '-graph');
      this.chart = new Chart(ctx, {
        type: system_time_chart.type,
        data: system_time_chart.data,
        options: system_time_chart.options
      });
    },

    updateChart() {
      if (!this.system.history_1m) {
        return;
      }
      this.setValues();
      this.chart.update();
    },

    time_spent() {
      let time_spent = this.system.current.time_spent;
      if (time_spent >= 1.0) {
        return time_spent.toFixed(0) + "s";
      }

      time_spent *= 1000;
      if (time_spent >= 1.0) {
        return time_spent.toFixed(0) + "ms";
      }

      time_spent *= 1000;
      if (time_spent >= 1.0) {
        return time_spent.toFixed(0) + "us";
      }

      time_spent *= 1000;
      if (time_spent >= 1.0) {
        return time_spent.toFixed(0) + "ns";
      } else if (time_spent >= 0.01) {
        return time_spent.toFixed(2) + "ns";
      } else {
        return "0ns";
      }
    },
    percentage_formatted() {
      let pct = this.percentage;
      if (pct > 10.0) {
        return pct.toFixed(0) + "%"
      } else {
        return pct.toFixed(1) + "%"
      }
    },
    percentage_class() {
      let pct = this.percentage;
      if (pct < 1.0) {
        return "pct-lowest";
      } else if (pct < 5.0) {
        return "pct-low";
      } else if (pct < 10.0) {
        return "pct-medium";
      } else if (pct < 20.0) {
        return "pct-high";
      } else {
        return "pct-highest";
      }
    }
  },
  template: `
    <div class="system-time-graph">
      <div class="system-graph-label">
        {{time_spent()}}
      </div>    
      <div :class="'system-graph-label-secondary ' + percentage_class()">
        {{percentage_formatted()}}
      </div>    
      <canvas :id="'system-time-' + system.name + '-graph'" :data-fps="tick"></canvas>
    </div>`
});
