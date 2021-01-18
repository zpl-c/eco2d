
Vue.component('entity-search', {
  props: [ "error", "search_text", "found" ],
  data: function() {
    return {
      search_scope: ""
    }
  },
  methods: {
    css() {
      var result = "entity-search";
      if (this.valid) {
        result += " entity-search-found";
      }
      return result;
    },
    add_entity: function(e) {
      this.$emit('add-entity', {entity: this.search_scope});
    },
    changed: function(e) {
      this.$emit('changed', {entity: e.target.value});
    },
    set_value(scope) {
      this.search_scope = scope;
    }
  },
  computed: {
    default_text: function() {
      if (this.search_text) {
        return this.search_text;
      } else {
        return "Find an entity";
      }
    },
    valid: function() {
      return this.found && this.search_scope.length;
    }
  },
  template: `
    <div :class="css()">
      <input v-on:keyup="changed" v-model="search_scope" :placeholder="default_text"></input>
      <img src="images/add.png" class="entity-remove-icon" v-if="!found && search_scope.length" v-on:click="add_entity">
    </div>
    `
});
