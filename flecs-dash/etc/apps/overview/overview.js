
Vue.component('app-overview', {
  mounted: function() {
    this.startRequesting("");
  },
  beforeDestroy: function() {
    this.stopRequesting();
  },  
  data: function() {
    return {
      data: {},
      data_request: undefined,
      error: false,
      tick: 0
    }
  },  
  methods: {
    request_data() {
      var url = "metrics?world=yes";

      this.tick ++;

      app.get(url, (msg) => {
        this.data = msg;
        this.error = false;
      }, (Http) => {
        if (Http.status == 404) {
          this.error = true;
        }
      });      
    },
    // Stop periodically requesting the scope
    stopRequesting() {
      this.data = [];
      clearInterval(this.data_request);
    },
    // Start periodically requesting a scope
    startRequesting(scope) {
      this.stopRequesting();

      // Initial request
      this.request_data();

      // Start periodic request
      this.data_request = window.setInterval(function() {
        this.request_data();
      }.bind(this), 1000);
    },
    entity_count() {
      if (!this.data.world) {
        return 0;
      }
      return this.data.world.current.entity_count;
    },
    operation_count() {
      if (!this.data.world) {
        return 0;
      }
      return (this.data.world.current.add_count + 
        this.data.world.current.remove_count +
        this.data.world.current.clear_count +
        this.data.world.current.delete_count +
        this.data.world.current.set_count +
        this.data.world.current.bulk_new_count).toFixed(0);
    },
    system_count() {
      if (!this.data.world) {
        return 0;
      }
      return (this.data.world.current.systems_ran).toFixed(0);
    },
    component_count() {
      if (!this.data.world) {
        return 0;
      }
      return this.data.world.current.component_count;
    },
    table_count() {
      if (!this.data.world) {
        return 0;
      }
      return this.data.world.current.table_count;
    },
    fragmentation() {
      if (!this.data.world) {
        return 0;
      }

      /* Compute global fragmentation as total matched tables divided by total 
       * matched entities. Subtract one from the tables, so that if there is a
       * single entity matching a single table, fragmentation is 0% */
      return (100 * (this.data.world.current.matched_table_count - 1) / 
          this.data.world.current.matched_entity_count).toFixed(0);
    }         
  },
  template: `
    <div class="overview">
      <div class="metric-row">

        <div class="metric metric-column">
          <div class="metric-header">entities</div>
          <div class="metric-content">
            {{entity_count()}}
          </div>
        </div>  

        <div class="metric metric-column">
          <div class="metric-header">components</div>
          <div class="metric-content">
            {{component_count()}}
          </div>
        </div>

        <div class="metric metric-column">
          <div class="metric-header">systems</div>
          <div class="metric-content">
            {{system_count()}}
          </div>
        </div>

        <div class="metric metric-column">
          <div class="metric-header">tables</div>
          <div class="metric-content">
            {{table_count()}}
          </div>
        </div>  

        <div class="metric metric-column">
          <div class="metric-header">fragmentation</div>
          <div class="metric-content">
            {{fragmentation()}}%
          </div>
        </div> 
       
      </div>

      <div class="metric">
        <div class="metric-header">performance</div>
        <div class="metric-content">
          <performance-graph :data="data" :tick="tick"></performance-graph>
        </div>
      </div>

      <div class="metric">
        <div class="metric-header">delta time</div>
        <div class="metric-content">
          <delta-time-graph :data="data" :tick="tick"></delta-time-graph>
        </div>
      </div>      

      <div class="metric">
        <div class="metric-header">operations ({{operation_count()}})</div>
        <div class="metric-content">
          <operation-graph :data="data" :tick="tick"></operation-graph>
        </div>
      </div>

      <div class="metric">
        <div class="metric-header">entities ({{entity_count()}})</div>
        <div class="metric-content">
          <entity-graph :data="data" :tick="tick"></entity-graph>
        </div>
      </div>

      <div class="metric">
        <div class="metric-header">tables ({{table_count()}})</div>
        <div class="metric-content">
          <table-graph :data="data" :tick="tick"></table-graph>
        </div>
      </div>      
    </div>
    `
});
  
app.app_loaded("overview", [{
  name: "entity-graph",
  url: "apps/overview/entity_graph.js"
}, {
  name: "performance-graph",
  url: "apps/overview/performance_graph.js"
}, {
  name: "operation-graph",
  url: "apps/overview/operation_graph.js"
}, {
  name: "table-graph",
  url: "apps/overview/table_graph.js"
}, {
  name: "delta-time-graph",
  url: "apps/overview/delta_time_graph.js"
}]);
