// Component that implements a traversable entity tree
Vue.component('entity-tree', {
  props: {
    filter: Array,
    search_text: String,
    auto_update: Boolean
  },
  mounted: function() {
    this.startRequesting("");
  },
  beforeDestroy: function() {
    this.stopRequesting();
  },  
  data: function() {
    return {
      scope_pending: "",
      scope: "",
      scope_info: {},
      scope_request: undefined,
      entities: [],
      entity: "",
      error: false
    }
  },
  methods: {
    // Send an HTTP request for the current scope
    request_scope(scope) {
      var url = "browse/" + scope + "?include=Name";
      if (this.filter) {
        url += "," + this.filter.join(",");
      }
      app.get(url, (msg) => {
        if (this.scope != this.scope_pending) {
          this.$emit("select-scope", {scope: this.scope_pending});
        }
        this.scope = this.scope_pending;
        this.entities = msg;
        this.error = false;
      }, (Http) => {
        if (Http.status == 404) {
          this.error = true;
        }
      });
    },
    // Send an HTTP request for the current scope
    request_scope_info(scope) {
      var filter = ""
      if (this.filter) {
        filter = "?include=" + this.filter.join(",");
      }
      app.get("info/" + scope + filter, (msg) => {
        this.scope_info = msg;
      }, (Http) => {
        if (Http.status == 404) {
          this.error = true;
        }
      });
    },  
    // Send an HTTP request for the current scope
    request_add_entity(entity) {
      app.post("entity/" + entity, (msg) => {
      }, (Http) => {
        if (Http.status == 404) {
          this.error = true;
        }
      });
    },
    // Stop periodically requesting the scope
    stopRequesting() {
      this.scope = undefined;
      this.scope_validated = undefined;
      clearInterval(this.scope_request);
    },
    // Start periodically requesting a scope
    startRequesting(scope) {
      this.stopRequesting();

      // Initial request
      var scope_url = scope.replace(/\./g, "/");
      this.request_scope(scope_url);
      this.request_scope_info(scope_url);

      // Start periodic request
      if (this.auto_update) {
        this.scope_request = window.setInterval(function() {
          this.request_scope(scope_url);
        }.bind(this), 1000);
      }
    },
    // Reset to initial state
    reset() {
      this.entity = "";
      this.entities = [];
      this.scope = "";
      this.scope_pending = "";
      this.$refs.search.set_value("");

      if (!this.auto_update) {
        this.startRequesting(this.scope);
      }
    },
    // Change current scope to fully qualified path
    set_scope(scope) {
      this.scope_pending = scope;
      this.$refs.search.set_value(scope);
      this.startRequesting(scope);

      if (this.entity != "") {
        this.select_entity("");
      }
    },
    // Event that contains an full path
    e_set_scope: function(event) {
      this.set_scope(event.entity);
    },
    // Event that contains a relative entity id
    e_set_scope_relative: function(event) {
      var scope = "";

      if (this.scope && this.scope != "") {
        scope = this.scope + "." + event.entity;
      } else {
        scope = event.entity;
      }

      this.set_scope(scope);
    },

    // Select an entity from the tree
    select_entity(entity) {
      if (entity == this.entity) {
        this.entity = "";
      } else {
        this.entity = entity;
      }

      this.$emit("select-entity", {entity: this.entity});
    },
    // Select the current scope
    e_select_scope: function() {
      this.select_entity(this.scope);
    },
    e_add_entity: function(e) {
      this.request_add_entity(e.entity);
      this.scope = this.scope_pending;
      var parent = this.parent;
      this.scope_pending = parent;
      this.startRequesting(parent);
      this.$refs.search.set_value(parent);
    },
    // Select a relative entity identifier
    e_select_entity_relative: function(event) {
      var entity;
      if (this.scope) {
        entity = this.scope + "." + event.entity;
      } else {
        entity = event.entity;
      }
      this.select_entity(entity);
    },
    entity_tree_css() {
      var result = "entity-tree-list";
      if (this.show_nav) {
        result += " entity-tree-list-container";
      }
      return result;
    }
  },
  computed: {
    parent: function() {
      if (this.scope && this.scope.indexOf(".")) {
        return this.scope.substr(0, this.scope.lastIndexOf("."));
      } else {
        return "";
      }
    }   
  },
  template: `
    <div class="entity-tree">
      <entity-search
          :found="scope === scope_pending" 
          :search_text="search_text" 
          v-on:changed="e_set_scope"
          v-on:add-entity="e_add_entity"
          ref="search">
      </entity-search>

      <div :class="entity_tree_css()">
        <entity-tree-item :entity="scope_info" :text="'.'" :current="entity" v-if="scope && scope.length"
          v-on:select-entity="e_select_scope">
        </entity-tree-item>

        <div class="entity-tree-separator" v-if="scope && scope.length">
        </div>

        <entity-tree-item :entity="{'name':parent, 'type':[]}" :text="'..'" nav_only="true" v-if="scope && scope.length"
          v-on:nav-scope="e_set_scope">
        </entity-tree-item>
        
        <entity-tree-item v-for="(e, i) in entities" :entity="e" :current="entity" :key="e.name"
          v-on:nav-scope="e_set_scope_relative"
          v-on:select-entity="e_select_entity_relative">
        </entity-tree-item>
      </div>
    </div>
    `
});
