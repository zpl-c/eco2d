
Vue.component('entity-data', {
  props: [ "filter" ],
  mounted: function() {
    this.startRequesting("");
  },
  beforeDestroy: function() {
    this.stopRequesting();
  },  
  data: function() {
    return {
      scope: "",
      scope_pending: "",
      data: [],
      data_request: undefined,
      error: false
    }
  },
  methods: {
    request_data(scope) {
      var url = "scope/" + scope + "?include=Name";
      if (this.filter) {
        url += "," + this.filter.join(",");
      }

      app.get(url, (msg) => {
        this.data = msg;
        this.error = false;
        this.scope = this.scope_pending;
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
      var scope_url = scope.replace(/\./g, "/");
      this.request_data(scope_url);

      // Start periodic request
      this.data_request = window.setInterval(function() {
        this.request_data(scope_url);
      }.bind(this), 1000);
    },
    select_scope(scope) {
      this.scope_pending = scope;
      this.startRequesting(scope);
    }   
  },
  template: `
    <div>
      <entity-table v-for="table in data" :data="table" :key="table.type.join(',')"></entity-table>
    </div>
    `
});
