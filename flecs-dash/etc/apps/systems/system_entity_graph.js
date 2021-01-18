
const system_entity_chart = {
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
        backgroundColor: [ '#26352c' ],
        borderColor: [ '#5BE595', ],
        borderWidth: 2,
        pointRadius: 0,
        fill: true,
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

Vue.component('system-entity-graph', {
  props: ['tick', 'system'],
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

      system_entity_chart.data.datasets[0].data = this.system.history_1m.matched_entity_count.avg;
    },

    createChart() {
      const ctx = document.getElementById('system-entity-' + this.system.name + '-graph');
      this.chart = new Chart(ctx, {
        type: system_entity_chart.type,
        data: system_entity_chart.data,
        options: system_entity_chart.options
      });
    },

    updateChart() {
      if (!this.system.history_1m) {
        return;
      }
      this.setValues();
      this.chart.update();
    },

    entity_count() {
      if (!this.system || !this.system.current) {
        return;
      }
      return this.system.current.matched_entity_count;
    }
  },
  template: `
    <div class="system-entity-graph">
      <div class="system-graph-label system-graph-label-filled">
        {{entity_count()}}
      </div>
      <canvas :id="'system-entity-' + system.name + '-graph'" :data-fps="tick"></canvas>
    </div>`
});
