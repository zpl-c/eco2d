Vue.component('sidebar', {
  props: {
    app: String,
    apps: Array
  },
  methods: {
    select_app(event) {
      this.$emit("app-select", event);
    }
  },
  computed: {
    app_names: function() {
      let ids = [];

      if (!this.apps) {
        return;
      }

      this.apps.forEach((table) => {
        if (table.data && table.data.Name) {
          table.data.Name.forEach((app) => {
            ids.push(app.value);
          });
        }
      });

      return ids;
    },
    app_icons: function() {
      let ids = [];

      if (!this.apps) {
        return;
      }

      this.apps.forEach((table) => {
        if (table.data && table.data["flecs.dash.App"]) {
          table.data["flecs.dash.App"].forEach((app) => {
            ids.push(app.icon);
          });
        }
      });

      return ids;
    }    
  },
  template: `
    <div class="sidebar">
      <sidebar-button v-for="(el, i) in app_names" 
        :app="el"
        :icon="app_icons[i]"
        :active_app="app" 
        :key="el"
        v-on:select-app="select_app">
      </sidebar-button>
    </div>
    `
});
  